#ifndef START_ENGINE_H
#define START_ENGINE_H

#include "build.h"
#include "declarations.h"

#ifdef BUILD_FOR_WINDOWS
#include <Windows.h>
#endif

#include <string>
#include <vector>
#include "GraphicsCore.h"
#include "Input.h"
#include "JPS.h"
#include "RayIntersection.h"
#include "Network.h"
#include "Grid.h"
#include "BoundingBox.h"
#include "SAT_BoundingBox.h"
#include "AStar.h"
#include <functional>
#include <mutex>

struct BoundingBox;

class StartEngine {
private:
	GraphicsCore gfx;
	Input input;
	float dSin[360], dCos[360];	//sin and cos values in degrees
	PathFinderJPS pathFinderJPS;
	PathFinderAStar pathFinderAStar;
	Network *network;
	int32 frameWidth, frameHeight;
	int gridOffsetX, gridOffsetY;
	std::function<void(int, int)> setCursorPositionCallback;
	std::function<void(bool)> showCursorCallback;
	int screenResolutionX, screenResolutionY;
	std::mutex networkMutex;

	void commonInit(int32 frameWidth, int32 frameHeight, std::function<void(int, int)> setCursorPositionCallback, 
		std::function<void(bool)> showCursorCallback);

public:
#ifdef BUILD_FOR_UNIX
    StartEngine(int32 frameWidth, int32 frameHeight, int32 *keys, std::function<void(int, int)> setCursorPositionCallback,
                std::function<void(bool)> showCursorCallback, int screenResolutionX, int screenResolutionY);
#endif
#ifdef BUILD_FOR_WINDOWS
	StartEngine(HWND *hwnd, int32 frameWidth, int32 frameHeight, int32 *keys, std::function<void(int, int)> setCursorPositionCallback, 
		std::function<void(bool)> showCursorCallback, int screenResolutionX, int screenResolutionY);
#endif
	~StartEngine();

	float sin(int degrees);
	float cos(int degrees);

	void getFrameSize(int32 &width, int32 &height);
	void getScreenResolution(int &x, int &y);

	//Graphics stuff
	void clearScreen();
	void clearScreen(uint32 color);
	void clearBackground(uint32 color);
	void draw(const Image *image, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int32 x, int32 y);
	void drawText(std::string text, const Image *fontPtr, int fontWidth, int fontHeight, int fontSpace, uint32 fontColor, int x, int y);
	void drawText(std::string text, const VariableSizeFont *variableSizeFont, int fontSpace, int characterPadding, uint32 fontColor, int x, int y);
	void drawBackground(const Image *image, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y);
	void presentScreen();
	uint32 *getFrameBuffer();
	uint32 *getBackgroundBuffer();
	uint32 alphaBlend(uint32 src, uint32 dest);
	void setOffsetCoordinates(float x, float y);
	void drawCircle(int x0, int y0, int radius, uint32 color, bool alpha);
	void drawLine(Vector2f *startPoint, Vector2f lookAt, int length, uint32 color);
	void drawLine(Vector2f startPoint, Vector2f lookAt, int length, uint32 color);
	void drawLineFade(Vector2f startPoint, Vector2f lookAt, int length, uint32 color);
	void drawThickBeamNoBoundCheck(Vector2f startPoint, Vector2f direction, int length, uint32 (&widthColors)[5]);
	void makeFrameMonochrome();
	void makeImageMonochrome(Image *image);

	void clearHUDBuffer(int color);
	void drawHUD(const Image *image, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int32 x, int32 y);
	void drawTextHUD(std::string text, const Image *fontPtr, int fontWidth, int fontHeight, int fontSpace, uint32 fontColor, int x, int y);
	void makeRectAreaDarkerHUD(Vector2i start, Vector2i end, int divisor);
	void setRenderHUD(bool render);
	void presentHUD();

	void displayGraphics();

	//Input
	Input *getInputObject();

	bool isKeyPressed(uint32 key);
	bool wasKeyToggled(uint32 key);
	bool isAsciiKeyPressed(int asciiKey);

	void getMouseXY(uint16 &x, uint16 &y);
	void setMouseCursorInScreenSpace(int x, int y);
	bool isMouseLeftPressed();
	bool isMouseMiddlePressed();
	bool isMouseRightPressed();
	bool wasMouseLeftToggled();
	bool wasMouseMiddleToggled();
	bool wasMouseRightToggled();
	void showCursor(bool show);

	bool isCursorInsideBB(const BoundingBox *bBox);
	void resetInputToggles();

	//Collision
	bool checkCollision(const BoundingBox *bb1, const  BoundingBox *bb2);
	bool checkCollisionBottom(const BoundingBox *bbObject, const  BoundingBox *bBox);
	bool checkCollisionTop(const BoundingBox *bbObject, const  BoundingBox *bBox);
	bool checkCollisionSAT(const SATBoundingBox *b1, const SATBoundingBox *b2);
	bool testCollision(const Vector2f *p1, const Vector2f *p2, float radius1, float radius2);

	bool checkIfLineSegmentIntersectsBB(const RayIntersection::Line *line, const BoundingBox *bBox);

	//Path finding
	void createGridMap(Grid ***gridMap, const std::vector<BoundingBox> *bBoxes, int gridWidth, int gridHeight, bool allocateMemory = true);
	void destroyGridMap(Grid **gridMap, int gridWidth);
	void findPath(Vector2f start, Vector2f goal, Grid **gridMap, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut);
	void findPathStraightMovesOnly(Vector2f start, Vector2f goal, Grid **gridMap, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut);
	void setGridOffsets(int gridOffsetX, int gridOffsetY);

	//Network
	bool initialiseClientNetwork(std::string serverIp, int serverPort, int port, std::function<void(uint8*)> packetRecievedCallback);
	bool initialiseServerNetwork(int port, std::function<void(uint8*)> packetRecievedCallback);
	void closeNetwork();
	void sendDataPacket(uint8 *packet, int packetSize);
	void sendDataPacket(uint8 *packet, int packetSize, sockaddr_in clientAddr);
	sockaddr_in getClientAddress();
	void clientEnableBroadcast();
	void clientDisableBroadcast();
};

#endif
