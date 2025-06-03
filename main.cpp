#include "build.h"
#include "declarations.h"

#ifdef BUILD_FOR_WINDOWS
#include <Windows.h>
#endif

#include "StartEngine.h"
#include "HardCodedLevel.h"
#include "timer.h"
#include <time.h>
#include "MT_RandomGenerator.h"
#include "ScreenManager.h"
#include "FrameCountTimer.h"
#include "SfxPool.h"
#include "CrossPlatformSleep.h"
#include "ConfigFile.h"
#include <exception>
using namespace CrossPlatformSleep;

#ifdef BUILD_FOR_UNIX
#include <GLFW/glfw3.h>
#include <unistd.h>
#endif

#ifdef BUILD_FOR_WINDOWS
#ifdef WINDOWS_DEBUG_MODE
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

#include "CampaignGameManager.h"
#include "ClientNetworkGameManager.h"
#include "HostNetworkGameManager.h"

StartEngine *engine;

#ifdef BUILD_FOR_UNIX
GLFWwindow *window = 0;

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		engine->getInputObject()->setKeyStatus(key, true);
	} else {
		engine->getInputObject()->setKeyStatus(key, false);
	}
}

static void cursorPositionCallback(GLFWwindow *window, double x, double y) {
	engine->getInputObject()->setMouseXY(x, y);
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (button) {
			case GLFW_MOUSE_BUTTON_LEFT:
				engine->getInputObject()->setMouseLeftPressed(true);
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				engine->getInputObject()->setMouseRightPressed(true);
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				engine->getInputObject()->setMouseMiddlePressed(true);

		}
	} else {
		switch (button) {
			case GLFW_MOUSE_BUTTON_LEFT:
				engine->getInputObject()->setMouseLeftPressed(false);
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				engine->getInputObject()->setMouseRightPressed(false);
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				engine->getInputObject()->setMouseMiddlePressed(false);
		}
	}
}

void showCursor(bool show) {
	if (show) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	} else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
}

void setCursorPosition(int x, int y) {
	glfwSetCursorPos(window, x, y);
	engine->getInputObject()->setMouseXY(x, y);
}

int main() {
    srand(time(0));
	
    if (!glfwInit()) {
		//TODO: Error Handling
        return -1;
	}

    const GLFWvidmode *videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = glfwCreateWindow(videoMode->width, videoMode->height, "Chaos Shell", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL) {
        return -1;
    }
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    int32 keys[INPUT_KEY_MAX];
	keys[INPUT_KEY_DOWN] = 'S';
	keys[INPUT_KEY_UP] = 'W';
	keys[INPUT_KEY_LEFT] = 'A';
	keys[INPUT_KEY_RIGHT] = 'D';
	keys[INPUT_KEY_THROW_SPECIAL] = ' ';
    keys[INPUT_KEY_CURSOR_TOGGLE] = 'C'
	engine = new StartEngine(1024, 768, keys,
                 [](int x, int y) { setCursorPosition(x, y); }, [](bool show) { showCursor(show); },
                 videoMode->width, videoMode->height);
    engine->createSoundSources(5);
    SfxPool::initialise(engine);

	Immobiliser::addAnimationFrame("Images/immobiliser1.png");
	Immobiliser::addAnimationFrame("Images/immobiliser2.png");
	Immobiliser::addAnimationFrame("Images/immobiliser3.png");
	Immobiliser::addAnimationFrame("Images/immobiliser4.png");
	Immobiliser::addAnimationFrame("Images/immobiliser5.png");

    Explosion::getInstance(engine);

	Image pickupSprites[10], pickupNoRespawn, pickupCross;

	pickupSprites[PICKUP_GRANADE].loadImage("Images/specialAttacks/explosive.png");
	pickupSprites[PICKUP_HOMING].loadImage("Images/specialAttacks/homing.png");
	pickupSprites[PICKUP_IMMOBILISER].loadImage("Images/specialAttacks/stun.png");
	pickupSprites[PICKUP_PINBALL].loadImage("Images/specialAttacks/pinball.png");
	pickupSprites[PICKUP_SHIELD].loadImage("Images/specialAttacks/shield.png");
	pickupSprites[PICKUP_PIERCING_BALL].loadImage("Images/specialAttacks/piercingBall.png");
	pickupSprites[PICKUP_BATTERY].loadImage("Images/specialAttacks/battery.png");
	pickupSprites[PICKUP_INVISIBLE_BALL].loadImage("Images/specialAttacks/invisibleBall.png");
	pickupSprites[PICKUP_TELEPORT_BALL].loadImage("Images/specialAttacks/teleport.png");
	pickupSprites[PICKUP_CALL_BALL].loadImage("Images/specialAttacks/call.png");
	pickupNoRespawn.loadImage("Images/specialAttacks/noRespawn.png");
	pickupCross.loadImage("Images/specialAttacks/pickupItemCross.png");

	GLFWwindow *win = window;
	ScreenManager *screenManager = new ScreenManager(engine, [win]() { glfwSetWindowShouldClose(window, true); });

	Timer timer(16);
	while (!glfwWindowShouldClose(window)) {
        if (timer.checkTimer()) {
			screenManager->renderScreen();
            engine->displayGraphics();
            glfwSwapBuffers(window);
			glfwPollEvents();
            FrameCountTimer::incrementTotalFrames();
			engine->updateAllSoundSources();
        } else {
			crossPlatformSleep(1);
		}
	}

	delete screenManager;
	engine->flushAllSources();
	SfxPool::cleanUp(engine);
	delete engine;
	glfwDestroyWindow(window);
	glfwTerminate();
}
#endif

#ifdef BUILD_FOR_WINDOWS

const char *WND_CN = "Start2DBattleBallCN";

void quitGame();

void showCursor(bool show) {
	if (show) {
		while (ShowCursor(true) < 0) { }
	} else {
		while (ShowCursor(false) >= 0) { }
	}
}

void setCursorPosition(int x, int y) {
	SetCursorPos(x, y);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msG, WPARAM wParam, LPARAM lParam) {
	switch (msG) {
		case WM_KEYUP:
			engine->getInputObject()->setKeyStatus(wParam, false);
			break;
		case WM_KEYDOWN:
			engine->getInputObject()->setKeyStatus(wParam, true);
			break;
		case WM_MOUSEMOVE:
			engine->getInputObject()->setMouseXY(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_LBUTTONDOWN:
			engine->getInputObject()->setMouseLeftPressed(true);
			break;
		case WM_LBUTTONUP:
			engine->getInputObject()->setMouseLeftPressed(false);
			break;
		case WM_MBUTTONDOWN:
			engine->getInputObject()->setMouseMiddlePressed(true);
			break;
		case WM_MBUTTONUP:
			engine->getInputObject()->setMouseMiddlePressed(false);
			break;
		case WM_RBUTTONDOWN:
			engine->getInputObject()->setMouseRightPressed(true);
			break;
		case WM_RBUTTONUP:
			engine->getInputObject()->setMouseRightPressed(false);
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msG, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
#ifdef WINDOWS_DEBUG_MODE
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	MT_RandomGenerator::seed(time(0));
	ConfigFile::loadConfigFile();

	WNDCLASSEX wC = {0};

	wC.cbSize = sizeof(WNDCLASSEX);
	wC.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wC.hCursor = LoadCursor(NULL, IDC_ARROW);
	wC.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wC.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPLICATION), IMAGE_ICON,16, 16, 0); 
	wC.hInstance = hInstance;
	wC.lpfnWndProc = WndProc;
	wC.lpszClassName = WND_CN;

	if (!RegisterClassEx(&wC)) {
		MessageBox(NULL, "Could not register Window Class", "Error", MB_ICONERROR);
		return 0;
	}
	
	uint32 screenResolutionX, screenResolutionY;
	ConfigFile::getScreenResolution(screenResolutionX, screenResolutionY);
	
	if (screenResolutionX == 0 && screenResolutionY == 0) {
		screenResolutionX = GetSystemMetrics(SM_CXSCREEN);
		screenResolutionY = GetSystemMetrics(SM_CYSCREEN);
	}
	HWND hwnd = CreateWindowEx(0, WND_CN, "Start 2D Engine", WS_POPUP /*| WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX*/, 
		0, 0, screenResolutionX, screenResolutionY, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, "Could not create Window", "Error", MB_ICONERROR);
		return 0;
	}

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	MSG Msg;
	Timer timer(16);
	
	int32 keys[INPUT_KEY_MAX];
	keys[INPUT_KEY_DOWN] = 'S';
	keys[INPUT_KEY_UP] = 'W';
	keys[INPUT_KEY_LEFT] = 'A';
	keys[INPUT_KEY_RIGHT] = 'D';
	keys[INPUT_KEY_THROW_SPECIAL] = ' ';
	keys[INPUT_KEY_CURSOR_TOGGLE] = 'C';
	engine = 0;
	ScreenManager *screenManager = 0;

	try {
		engine = new StartEngine(&hwnd, 1024, 768, keys, [](int x, int y) { setCursorPosition(x, y); }, [](bool show) { showCursor(show); },
								screenResolutionX, screenResolutionY);
		engine->createSoundSources(5);
		SfxPool::initialise(engine);

		Immobiliser::addAnimationFrame("Images/immobiliser1.png");
		Immobiliser::addAnimationFrame("Images/immobiliser2.png");
		Immobiliser::addAnimationFrame("Images/immobiliser3.png");
		Immobiliser::addAnimationFrame("Images/immobiliser4.png");
		Immobiliser::addAnimationFrame("Images/immobiliser5.png");

		Explosion::getInstance(engine);

		//Image pickupSprites[10], pickupNoRespawn, pickupCross;

		/*pickupSprites[PICKUP_GRANADE].loadImage("Images/specialAttacks/explosive.png");
		pickupSprites[PICKUP_HOMING].loadImage("Images/specialAttacks/homing.png");
		pickupSprites[PICKUP_IMMOBILISER].loadImage("Images/specialAttacks/stun.png");
		pickupSprites[PICKUP_PINBALL].loadImage("Images/specialAttacks/pinball.png");
		pickupSprites[PICKUP_SHIELD].loadImage("Images/specialAttacks/shield.png");
		pickupSprites[PICKUP_PIERCING_BALL].loadImage("Images/specialAttacks/piercingBall.png");
		pickupSprites[PICKUP_BATTERY].loadImage("Images/specialAttacks/battery.png");
		pickupSprites[PICKUP_INVISIBLE_BALL].loadImage("Images/specialAttacks/invisibleBall.png");
		pickupSprites[PICKUP_TELEPORT_BALL].loadImage("Images/specialAttacks/teleport.png");
		pickupSprites[PICKUP_CALL_BALL].loadImage("Images/specialAttacks/call.png");
		pickupNoRespawn.loadImage("Images/specialAttacks/noRespawn.png");
		pickupCross.loadImage("Images/specialAttacks/pickupItemCross.png");*/

		screenManager = new ScreenManager(engine, quitGame);
		while(TRUE) {
			while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
				if (Msg.message == WM_QUIT) {
					goto gameLoopEnd;
				}
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
			if (timer.checkTimer()) {
				screenManager->renderScreen();
				FrameCountTimer::incrementTotalFrames();
				engine->displayGraphics();
				engine->updateAllSoundSources();
			} else {
				crossPlatformSleep(1);
			}
		}
	} catch (std::exception &e) {
		std::string errorMessage = e.what();
		errorMessage = "Fatal Error: " + errorMessage;
		MessageBox(hwnd, errorMessage.c_str(), "Error", MB_ICONERROR);
	}
gameLoopEnd:
	if (screenManager) {
		delete screenManager;
	}
	if (engine) {
		engine->flushAllSources();
		SfxPool::cleanUp(engine);
		delete engine;
	}
	return 0;
}

void quitGame() {
	PostQuitMessage(0);
}

#endif
