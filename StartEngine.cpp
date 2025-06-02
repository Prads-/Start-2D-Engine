#include "StartEngine.h"
#include "Collision.h"
#include <cmath>
#include "DynamicMultidimensionalArray.h"
using namespace DynamicMultidimensionalArray;
using namespace std;

#ifdef BUILD_FOR_UNIX
StartEngine::StartEngine(int32 frameWidth, int32 frameHeight, int32 *keys, std::function<void(int, int)> setCursorPositionCallback, std::function<void(bool)> showCursorCallback, int screenResolutionX, int screenResolutionY)
            : gfx(frameWidth, frameHeight, screenResolutionX, screenResolutionY), input(keys), pathFinderJPS(frameWidth, frameHeight), pathFinderAStar(frameWidth, frameHeight) {
	this->screenResolutionX = screenResolutionX;
	this->screenResolutionY = screenResolutionY;
	commonInit(frameWidth, frameHeight, setCursorPositionCallback, showCursorCallback, preserveAspectRatio);
}
#endif

#ifdef BUILD_FOR_WINDOWS
StartEngine::StartEngine(HWND *hwnd, int32 frameWidth, int32 frameHeight, int32 *keys, std::function<void(int, int)> setCursorPositionCallback, 
						 std::function<void(bool)> showCursorCallback, int screenResolutionX, int screenResolutionY) 
			: gfx(hwnd, frameWidth, frameHeight, screenResolutionX, screenResolutionY), input(keys), 
			pathFinderJPS(frameWidth, frameHeight), pathFinderAStar(frameWidth, frameHeight) {
	this->screenResolutionX = screenResolutionX;
	this->screenResolutionY = screenResolutionY;
	commonInit(frameWidth, frameHeight, setCursorPositionCallback, showCursorCallback);
}
#endif

StartEngine::~StartEngine() {
	closeNetwork();
}

void StartEngine::commonInit(int32 frameWidth, int32 frameHeight, std::function<void(int, int)> setCursorPositionCallback, 
							 std::function<void(bool)> showCursorCallback) {
	for (int i = 0; i < 360; ++i) {
		float radian = i * 3.14159265f / 180.0f;
		dSin[i] = std::sin(radian);
		dCos[i] = std::cos(radian);
	}
	network = 0;
	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
	gridOffsetX = 0;
	gridOffsetY = 0;
	this->setCursorPositionCallback = setCursorPositionCallback;
	this->showCursorCallback = showCursorCallback;

	int drawSpaceResolutionX = screenResolutionX, drawSpaceResolutionY = screenResolutionY;
	gfx.maintainAspectRatio(drawSpaceResolutionX, drawSpaceResolutionY);
	BoundingBox drawSpaceBBox;
	drawSpaceBBox.minP = Vector2f((screenResolutionX - drawSpaceResolutionX)  / 2.0f, (screenResolutionY - drawSpaceResolutionY) / 2.0f);
	drawSpaceBBox.maxP = Vector2f(drawSpaceResolutionX + (screenResolutionX - drawSpaceResolutionX)  / 2.0f, drawSpaceResolutionY + (screenResolutionY - drawSpaceResolutionY) / 2.0f);
	input.setDrawSpaceBBox(drawSpaceBBox);
	input.setMouseMultiplierXY((float)frameWidth / drawSpaceResolutionX, (float)frameHeight / drawSpaceResolutionY);
}

void StartEngine::clearScreen() {
	gfx.clearFrameBuffer();
}

void StartEngine::clearScreen(uint32 color) {
	gfx.clearFrameBuffer(color);
}

void StartEngine::clearBackground(uint32 color) {
	gfx.clearBackgroundBuffer(color);
}

void StartEngine::draw(const Image *image, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y) {
	gfx.draw(image, rotationAngle, flipX, flipY, alphaBlend, x, y);
}

void StartEngine::drawText(std::string text, const Image *fontPtr, int fontWidth, int fontHeight, int fontSpace, uint32 fontColor, int x, int y) {
	gfx.drawText(text, fontPtr, fontWidth, fontHeight, fontSpace, fontColor, x, y);
}

void StartEngine::drawText(std::string text, const VariableSizeFont *variableSizeFont, int fontSpace, int characterPadding, uint32 fontColor, int x, int y) {
	gfx.drawText(text, variableSizeFont, fontSpace, characterPadding, fontColor, x, y);
}

void StartEngine::drawBackground(const Image *image, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y) {
	gfx.drawBackground(image, rotationAngle, flipX, flipY, alphaBlend, x, y);
}

void StartEngine::presentScreen() {
	gfx.presentBuffer();
}

uint32 *StartEngine::getFrameBuffer() {
	return gfx.getFrameBuffer();
}

uint32 *StartEngine::getBackgroundBuffer() {
	return gfx.getBackgroundBuffer();
}

uint32 StartEngine::alphaBlend(uint32 src, uint32 dest) {
	return gfx.alphaBlend(src, dest);
}

void StartEngine::setOffsetCoordinates(float x, float y) {
	gfx.setOffsetCoordinates(x, y);
}

void StartEngine::drawCircle(int x0, int y0, int radius, uint32 color, bool alpha) {
	gfx.drawCircle(x0, y0, radius, color, alpha);
}

void StartEngine::drawLine(Vector2f *startPoint, Vector2f lookAt, int length, uint32 color) {
	gfx.drawLine(startPoint, lookAt, length, color);
}

void StartEngine::drawLine(Vector2f startPoint, Vector2f lookAt, int length, uint32 color) {
	gfx.drawLine(&startPoint, lookAt, length, color);
}

void StartEngine::drawLineFade(Vector2f startPoint, Vector2f lookAt, int length, uint32 color) {
	gfx.drawLineFade(startPoint, lookAt, length, color);
}

void StartEngine::drawThickBeamNoBoundCheck(Vector2f startPoint, Vector2f direction, int length, uint32 (&widthColors)[5]) {
	gfx.drawThickBeamNoBoundCheck(startPoint, direction, length, widthColors);
}

void StartEngine::makeFrameMonochrome() {
	gfx.makeFrameMonochrome();
}

void StartEngine::makeImageMonochrome(Image *image) {
	gfx.makeImageMonochrome(image);
}

void StartEngine::clearHUDBuffer(int color) {
	gfx.clearHUDBuffer(color);
}

void StartEngine::drawHUD(const Image *image, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int32 x, int32 y) {
	gfx.drawHUD(image, rotationAngle, flipX, flipY, alphaBlend, x, y);
}

void StartEngine::drawTextHUD(std::string text, const Image *fontPtr, int fontWidth, int fontHeight, int fontSpace, uint32 fontColor, int x, int y) {
	gfx.drawTextHUD(text, fontPtr, fontWidth, fontHeight, fontSpace, fontColor, x, y);
}

void StartEngine::makeRectAreaDarkerHUD(Vector2i start, Vector2i end, int divisor) {
	gfx.makeRectAreaDarkerHUD(start, end, divisor);
}

void StartEngine::setRenderHUD(bool render) {
	gfx.setRenderHUD(render);
}

void StartEngine::presentHUD() {
	gfx.presentHUD();
}

void StartEngine::displayGraphics() {
	gfx.render();
}

Input *StartEngine::getInputObject() {
	return &input;
}

bool StartEngine::isKeyPressed(uint32 key) {
	return input.isKeyPressed(key);
}

bool StartEngine::wasKeyToggled(uint32 key) {
	return input.wasKeyToggled(key);
}

bool StartEngine::isAsciiKeyPressed(int asciiKey) {
	return input.isAsciiKeyPressed(asciiKey);
}

void StartEngine::getMouseXY(uint16 &x, uint16 &y) {
	input.getMouseXY(x, y);
}

void StartEngine::setMouseCursorInScreenSpace(int x, int y) {
    setCursorPositionCallback(x, y);
}

bool StartEngine::isMouseLeftPressed() {
	return input.isMouseLeftPressed();
}

bool StartEngine::isMouseMiddlePressed() {
	return input.isMouseMiddlePressed();
}

bool StartEngine::isMouseRightPressed() {
	return input.isMouseRightPressed();
}

bool StartEngine::wasMouseLeftToggled() {
	return input.wasMouseLeftToggled();
}

bool StartEngine::wasMouseMiddleToggled() {
	return input.wasMouseMiddleToggled();
}

bool StartEngine::wasMouseRightToggled() {
	return input.wasMouseRightToggled();
}

void StartEngine::showCursor(bool show) {
	showCursorCallback(show);
}

bool StartEngine::isCursorInsideBB(const BoundingBox *bBox) {
	return input.isCursorInsideBBox(bBox);
}

void StartEngine::resetInputToggles() {
	input.resetInputToggles();
}

bool StartEngine::checkCollision(const BoundingBox *bb1, const BoundingBox *bb2) {
	return Collision::testCollision(bb1, bb2);
}

bool StartEngine::checkCollisionBottom(const BoundingBox *bbObject, const BoundingBox *bBox) {
	return Collision::testCollisionBottom(bbObject, bBox);
}

bool StartEngine::checkCollisionTop(const BoundingBox *bbObject, const BoundingBox *bBox) {
	return Collision::testCollisionTop(bbObject, bBox);
}

bool StartEngine::checkCollisionSAT(const SATBoundingBox *b1, const SATBoundingBox *b2) {
	return Collision::testCollisionSAT(b1, b2);
}

bool StartEngine::testCollision(const Vector2f *p1, const Vector2f *p2, float radius1, float radius2) {
	return Collision::testCollision(p1, p2, radius1, radius2);
}

bool StartEngine::checkIfLineSegmentIntersectsBB(const RayIntersection::Line *line, const BoundingBox *bBox) {
	return RayIntersection::checkIfLineSegmentIntersectsBB(line, bBox);
}

void StartEngine::createGridMap(Grid ***gridMap, const std::vector<BoundingBox> *bBoxes, int gridWidth, int gridHeight, bool allocateMemory) {
	int column = frameWidth, row = frameHeight;
	column = (column / gridWidth) + 2;
	row = (row / gridHeight) + 2;
	if (allocateMemory) {
		*gridMap = allocDynamicMultidimensionalArray<Grid>(row, column);
	}
	for (int y = 1; y < row; ++y) {
		for (int x = 1; x < column; ++x) {
			(*gridMap)[x][y].isObstacle = false;
		}
	}
	for (int i = 0; i < column; ++i) {
		(*gridMap)[i][0].isObstacle = true;
	}
	for (int i = 0; i < row; ++i) {
		(*gridMap)[0][i].isObstacle = true;
	}

	for (const BoundingBox &bBox : *bBoxes) {
        GridIndexBase min, max;
        min.x = bBox.minP.x;
		min.y = bBox.minP.y;
        max.x = bBox.maxP.x;
		max.y = bBox.maxP.y;

		if (max.x < gridOffsetX || max.y < gridOffsetY) {
			continue;
		}
		if (min.x < gridOffsetX) {
			min.x = gridOffsetX;
		}
		if (min.y < gridOffsetY) {
			min.y = gridOffsetY;
		}

        Vector2f position(min.x, min.y);
        GridIndexBase::positionToGridIndex(&position, gridWidth, gridHeight, gridOffsetX, gridOffsetY, &min);
        position = Vector2f(max.x, max.y);
        GridIndexBase::positionToGridIndex(&position, gridWidth, gridHeight, gridOffsetX, gridOffsetY, &max);

        for (int  y = min.y; y <= max.y; ++y) {
            for (int x = min.x; x <= max.x; ++x) {
                (*gridMap)[x][y].isObstacle = true;
            }
        }
    }
}

void StartEngine::destroyGridMap(Grid **gridMap, int gridWidth) {
	freeDynamicMultidimensionalArray(gridMap, (frameWidth / gridWidth) + 1);
}

void StartEngine::findPath(Vector2f start, Vector2f goal, Grid **gridMap, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut) {
	pathFinderJPS.findPath(&start, &goal, gridMap, gridWidth, gridHeight, pathOut);
}

void StartEngine::findPathStraightMovesOnly(Vector2f start, Vector2f goal, Grid **gridMap, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut) {
	pathFinderAStar.findPath(&start, &goal, gridMap, gridWidth, gridHeight, pathOut);
}

void StartEngine::setGridOffsets(int gridOffsetX, int gridOffsetY) {
	this->gridOffsetX = gridOffsetX;
	this->gridOffsetY = gridOffsetY;
	pathFinderAStar.setGridOffsets(gridOffsetX, gridOffsetY);
	pathFinderJPS.setGridOffsets(gridOffsetX, gridOffsetY);
}

float StartEngine::sin(int degrees) {
	return dSin[degrees % 360];
}

float StartEngine::cos(int degrees) {
	return dCos[degrees % 360];
}

void StartEngine::getFrameSize(int32 &width, int32 &height) {
	width = frameWidth;
	height = frameHeight;
}

void StartEngine::getScreenResolution(int &x, int &y) {
	x = screenResolutionX;
	y = screenResolutionY;
}

bool StartEngine::initialiseClientNetwork(std::string serverIp, int serverPort, int port, std::function<void(uint8*)> packetRecievedCallback) {
	closeNetwork();
	network = new ClientNetwork(serverIp, serverPort, port, packetRecievedCallback);
	if (!network->initialiseNetwork()) {
		closeNetwork();
		return false;
	}
}

bool StartEngine::initialiseServerNetwork(int port, std::function<void(uint8*)> packetRecievedCallback) {
	closeNetwork();
	network = new ServerNetwork(port, packetRecievedCallback);
	if (!network->initialiseNetwork()) {
		closeNetwork();
		return false;
	}
}

void StartEngine::closeNetwork() {
	networkMutex.lock();
	if (network) {
		delete network;
		network = 0;
	}
	networkMutex.unlock();
}

void StartEngine::sendDataPacket(uint8 *packet, int packetSize) {
	networkMutex.lock();
	if (network) {
		((ClientNetwork*)network)->sendPacket(packet, packetSize);
	}
	networkMutex.unlock();
}

void StartEngine::sendDataPacket(uint8 *packet, int packetSize, sockaddr_in clientAddr) {
	networkMutex.lock();
	if (network) {
		((ServerNetwork*)network)->sentPacket(packet, packetSize, clientAddr);
	}
	networkMutex.unlock();
}

sockaddr_in StartEngine::getClientAddress() {
	networkMutex.lock();
	sockaddr_in retVal;
	if (network) {
		retVal = ((ServerNetwork*)network)->getClientAddress();
	}
	networkMutex.unlock();
	return retVal;
}

void StartEngine::clientEnableBroadcast() {
	networkMutex.lock();
	if (network) {
		((ClientNetwork*)network)->clientEnableBroadcast();
	}
	networkMutex.unlock();
}

void StartEngine::clientDisableBroadcast() {
	networkMutex.lock();
	if (network) {
		((ClientNetwork*)network)->clientDisableBroadcast();
	}
	networkMutex.unlock();
}