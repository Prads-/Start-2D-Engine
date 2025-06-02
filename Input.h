#ifndef INPUT_CONTROLLER_H
#define INPUT_CONTROLLER_H

#include "build.h"
#include "declarations.h"
#include "BoundingBox.h"
#ifdef BUILD_FOR_UNIX
#include <GLFW/glfw3.h>
#endif

#define INPUT_KEY_LEFT				0
#define INPUT_KEY_RIGHT				1
#define INPUT_KEY_UP				2
#define INPUT_KEY_DOWN				3
#define INPUT_KEY_MOVE_LEFT			4
#define INPUT_KEY_MOVE_RIGHT		5
#define INPUT_KEY_CURSOR_TOGGLE		6
#define INPUT_KEY_THROW_SPECIAL		7
#define INPUT_KEY_MAX				8

class Input {
private:
	int32 keys[INPUT_KEY_MAX];

#ifdef BUILD_FOR_WINDOWS
	bool keysStatus[256];
#else
	bool keysStatus[GLFW_KEY_LAST];
#endif
	bool keysPreviousState[INPUT_KEY_MAX];	//Pressed = true
	int mouseX, mouseY;
    float mXMultiplier, mYMultiplier;
	bool mouseLeftPressed, mouseMiddlePressed, mouseRightPressed, 
		previousMouseLeftPressed, previousMouseMiddlePressed, previousMouseRightPressed;
	BoundingBox drawSpaceBBox;

	bool isCursorInsideBBox(Vector2i cursor, const BoundingBox *bBox);

public:
    Input(int32 *keys);

	bool isKeyPressed(uint32 keyCode);
	bool wasKeyToggled(uint32 keyCode);
	bool isAsciiKeyPressed(int asciiCode);

	void setKeyStatus(int keyCode, bool isPressed);
	void setMouseXY(int x, int y);
	void setMouseLeftPressed(bool pressed);
	void setMouseMiddlePressed(bool pressed);
	void setMouseRightPressed(bool pressed);

	void getMouseXY(uint16 &x, uint16 &y);
	bool isMouseLeftPressed();
	bool isMouseMiddlePressed();
	bool isMouseRightPressed();
	bool wasMouseLeftToggled();
	bool wasMouseMiddleToggled();
	bool wasMouseRightToggled();

    void getMouseMultiplierXY(float &x, float &y);
	void setMouseMultiplierXY(float x, float y);

	void setDrawSpaceBBox(BoundingBox bBox);
	bool isCursorInsideBBox(const BoundingBox *bBox);
	void resetInputToggles();
};

#endif
