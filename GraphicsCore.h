#ifndef GRAPHICS_CORE_H
#define GRAPHICS_CORE_H

#include "build.h"
#include "declarations.h"
#include "vector2.h"
#include "VariableSizeFont.h"

#ifdef BUILD_FOR_WINDOWS
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#endif

#ifdef BUILD_FOR_UNIX
#include <GL/glew.h>
#endif

#include <string>
#include "Image.h"

#define FRAME_WIDTH			1024
#define FRAME_HEIGHT		768
#define FRAME_BUFFER_SIZE	(FRAME_WIDTH * FRAME_HEIGHT)

#define HUD_WIDTH	57
#define HUD_HEIGHT	102
#define HUD_BUFFER_SIZE		(HUD_WIDTH * HUD_HEIGHT)

class GraphicsCore {
private:
    uint32 *frameBuffers[2], *hudBuffer;
    uint32 *backgroundBuffer;
    int32 frameWidth, frameHeight, halfResolutionX, halfResolutionY;
    int32 frameSize;
    uint32 *currentActiveBuffer, *drawingBuffer;
	int32 activeBuffer;
	float xOffset, yOffset;
	int32 sinFixedPointTable[360], cosFixedPointTable[360];
	bool renderHUD;

#ifdef BUILD_FOR_UNIX
	//OpenGL stuffs
    GLuint frameTexture, hudTexture;

    void initOpenGL(int screenResolutionX, int screenResolutionY);
	void openGLCleanUp();
#endif

#ifdef BUILD_FOR_WINDOWS
	struct TDVertices {
		float x, y, z;
		float u, v;
	};

	IDirect3DDevice9 *d3dDevice;
	IDirect3DTexture9 *texScreen, *texHUD;
	IDirect3DVertexBuffer9 *vertexBuffer, *hudVertexBuffer;
	HWND *hwnd;
	int screenResolutionX, screenResolutionY;
	bool deviceIsLost;

	bool initializeDirectX(HWND *hwnd, int screenResolutionX, int screenResolutionY);
	void createScreenPlane();
	void createHUDPlane(float rX, float rY);
	void directXCleanUp();
	void deviceLost();
#endif

    void commonInit(int32 frameWidth, int32 frameHeight);
	void drawRotation(const Image *imgPtr, int angle, bool flipX, bool flipY, bool alpha, int x, int y);
	void drawNoRotation(const Image *imgPtr, int angle, bool flipX, bool flipY, bool alpha, int x, int y);

public:

#ifdef BUILD_FOR_UNIX
    GraphicsCore(int32 frameWidth, int32 frameHeight, int screenResolutionX, int screenResolutionY);
#endif
#ifdef BUILD_FOR_WINDOWS
	GraphicsCore(HWND *hwnd, int32 frameWidth, int32 frameHeight, int screenResolutionX, int screenResolutionY);
#endif
	~GraphicsCore();

	void setOffsetCoordinates(float x, float y);
	void clearFrameBuffer(int bgColor);
	void clearFrameBuffer();
	void drawText(std::string text, const Image *fontPtr, int fontWidth, int fontHeight, int fontSpace, uint32 fontColor, int x, int y);
	void drawText(std::string text, const VariableSizeFont *variableSizeFont, int fontSpace, int characterPadding, uint32 fontColor, int x, int y);
	void draw(const Image *imagePtr, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y);
	void presentBuffer();
	uint32 *getFrameBuffer();
	uint32 *getBackgroundBuffer();

	void clearBackgroundBuffer(int bgColor);
	void drawBackground(const Image *imagePtr, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y);

	uint32 alphaBlend(uint32 src, uint32 dest);
	void drawCircle(int x0, int y0, int radius, uint32 color, bool alpha);
	void setPixel(int x, int y, uint32 color, bool alpha);
	void drawLine(Vector2f *startPoint, Vector2f direction, int length, uint32 color);
	void drawLineFade(Vector2f startPoint, Vector2f direction, int length, uint32 color);
	void drawThickBeamNoBoundCheck(Vector2f startPoint, Vector2f direction, int length, uint32 (&widthColors)[5]);
	void makeFrameMonochrome();
	void makeImageMonochrome(Image *image);

	//HUD stuffs
	void clearHUDBuffer(int color);
	void drawHUD(const Image *imagePtr, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y);
	void drawTextHUD(std::string text, const Image *fontPtr, int fontWidth, int fontHeight, int fontSpace, uint32 fontColor, int x, int y);
	void makeRectAreaDarkerHUD(Vector2i start, Vector2i end, int divisor);
	void setRenderHUD(bool render);
	void presentHUD();

	void render();

	void maintainAspectRatio(int &screenResolutionX, int &screenResolutionY);
};

#endif
