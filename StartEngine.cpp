#include "StartEngine.h"
#include "Collision.h"
#include <cmath>
#include "DynamicMultidimensionalArray.h"
#include "ConfigFile.h"
using namespace DynamicMultidimensionalArray;
using namespace std;
using namespace FileLoader;

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
	flushAllSources();
	deleteSoundSources();
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
	if (ConfigFile::getMaintainAspectRatio()) {
		gfx.maintainAspectRatio(drawSpaceResolutionX, drawSpaceResolutionY);
	}
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

void StartEngine::createBackground(std::string path, std::vector<PrioritisedAsset> *prioritisedAssets, std::vector<Image*> *prioritisedImageCollector) {
	gfx.clearBackgroundBuffer(0);

	vector<LevelAsset> levelAssets;
	loadLevelAssetsFile("maps/" + path + "/staticAssets.txt", &levelAssets);
	for (const FileLoader::LevelAsset &levelAsset : levelAssets) {
		Image *image = new Image("maps/" + path + "/" + levelAsset.path);
		bool hasPrioritisedClone = false;
		for (const FileLoader::LevelAsset::Clone &clone : levelAsset.clones) {
			if (clone.isPrioritised) {
				prioritisedAssets->push_back(PrioritisedAsset(image, clone, BoundingBox(Vector2f(clone.x, clone.y), Vector2f(clone.x + image->getWidth(), clone.y + image->getHeight()))));
				hasPrioritisedClone = true;
			} else {
				gfx.drawBackground(image, clone.rotationAngle, clone.flipX, clone.flipY, clone.alphaBlend, clone.x, clone.y);
			}
		}
		if (!hasPrioritisedClone) {
			delete image;
		} else {
			prioritisedImageCollector->push_back(image);
		}
	}
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

void StartEngine::resetListener() {
    audio.resetListener();
}

void StartEngine::startAudioStreaming(std::string path) {
    audio.startStreaming(path);
}

void StartEngine::stopAudioStreaming() {
    audio.stopStreaming();
}

bool StartEngine::getIsAudioStreaming() {
    return audio.getIsStreaming();
}

void StartEngine::setStreamGain(float gain) {
	audio.setStreamGain(gain);
}

void StartEngine::createSoundSources(int count) {
    for (int i = 0; i < count; ++i) {
    	AudioCore::AudioSource audioSource;
    	audio.getSource(&audioSource);
    	audioSources.push_back(audioSource);
    }
}

void StartEngine::updateAllSoundSources() {
    for (AudioCore::AudioSource &source : audioSources) {
    	if (source.isPlaying) {
    		audio.updateSourcePlayingStatus(&source);
    	}
    }
}

void StartEngine::deleteSoundSources() {
    for (AudioCore::AudioSource &audioSource : audioSources) {
    	audio.deleteSource(&audioSource);
    }
    audioSources.clear();
}

bool StartEngine::createAudioBuffer(std::string path, AudioCore::AudioBuffer *bufferOut) {
    return audio.createAudioBuffer(path, bufferOut);
}

void StartEngine::playAudioBuffer(AudioCore::AudioBuffer *buffer, bool applyParameters) {
    for (size_t i = 0; i < audioSources.size(); ++i) {
    	if (!audioSources[i].isPlaying) {
    		if (audioSources[i].attachedBuffer != buffer->buffer) {
    			audio.bindBufferToSource(&audioSources[i], buffer);
    		}
    		buffer->lastBoundSourceId = i;
    		if (applyParameters) {
    			audio.loopSource(&audioSources[i], buffer->loop);
    			audio.setSourceGain(&audioSources[i], buffer->gain);
    			audio.setSourcePitch(&audioSources[i], buffer->pitch);
    		}
    		audio.playSource(&audioSources[i]);
    		break;
    	}
    }
}

void StartEngine::stopAudioBuffer(const AudioCore::AudioBuffer *buffer) {
    if (buffer->lastBoundSourceId < audioSources.size()) {
    	if (audioSources[buffer->lastBoundSourceId].isPlaying && audioSources[buffer->lastBoundSourceId].attachedBuffer == buffer->buffer) {
    		audio.stopSource(&audioSources[buffer->lastBoundSourceId]);
    	}
    }
}

void StartEngine::flushAllSources() {
    for (AudioCore::AudioSource &source : audioSources) {
    	if (source.isPlaying) {
    		audio.stopSource(&source);
    	}
    	audio.bindBufferToSource(&source, (ALuint)0);
    }
}

void StartEngine::deleteAudioBuffer(AudioCore::AudioBuffer *buffer) {
    audio.deleteAudioBuffer(buffer);
}

void StartEngine::setSourcePosition(uint32 sourceId, Vector2f position) {
    if (sourceId < audioSources.size()) {
    	audio.setSourcePosition(&audioSources[sourceId], position);
    }
}

void StartEngine::loopSource(uint32 sourceId, bool loop) {
    if (sourceId < audioSources.size()) {
    	audio.loopSource(&audioSources[sourceId], loop);
    }
}

void StartEngine::setSourcePitch(uint32 sourceId, float pitch) {
    if (sourceId < audioSources.size()) {
    	audio.setSourcePitch(&audioSources[sourceId], pitch);
    }
}

void StartEngine::setSourceGain(uint32 sourceId, float gain) {
    if (sourceId < audioSources.size()) {
    	audio.setSourceGain(&audioSources[sourceId], gain);
    }
}

void StartEngine::setSourcesGain(float gain) {
	for (AudioCore::AudioSource &source : audioSources) {
		audio.setSourceGain(&source, gain);
	}
}