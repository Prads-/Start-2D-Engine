#include "Animation.h"
#include <stdexcept>
#include <fstream>
#include "stringExtension.h"
using namespace std;
using namespace StringExtension;

Animation::Animation() {
	commonInit();
	loop = false;
}

Animation::Animation(uint32 interval, bool loop) : intervalTimer(interval) {
	commonInit();
	this->loop = loop;
}

Animation::Animation(std::string path, bool loop) {
	commonInit();
	openAnimationFile(path);
	this->loop = loop;
}

Animation::~Animation() {
	if (frameLoadState == FRAME_LOAD_STATE_LOADED) {
		for (const Image *frame : animationFrames) {
			delete frame;
		}
	}
}

void Animation::commonInit() {
	currentFrame = 0;
	frameLoadState = FRAME_LOAD_STATE_NONE;
}
	
void Animation::addFrames(std::string *paths, int numOfPaths) {
	if (frameLoadState != FRAME_LOAD_STATE_SET) {
		for (int i = 0; i < numOfPaths; ++i) {
			Image *image = new Image(&paths[i]);
			animationFrames.push_back(image);
		}
		frameLoadState = FRAME_LOAD_STATE_LOADED;
	}
}

void Animation::addFrame(std::string path) {
	if (frameLoadState != FRAME_LOAD_STATE_SET) {
		Image *image = new Image(&path);
		animationFrames.push_back(image);
		frameLoadState = FRAME_LOAD_STATE_LOADED;
	}
}

void Animation::addFrame(const Image *frame) {
	if (frameLoadState != FRAME_LOAD_STATE_LOADED) {
		animationFrames.push_back(frame);
		frameLoadState = FRAME_LOAD_STATE_SET;
	}
}

void Animation::openAnimationFile(std::string path, bool clear) {
	if (frameLoadState != FRAME_LOAD_STATE_SET) {
		ifstream animationFile(path);
		if (animationFile.is_open()) {
			if (clear) {
				animationFrames.clear();
			}
			string pathFolder;
			unsigned int slashLocation = path.rfind('/');
			pathFolder = path.substr(0, slashLocation + 1);
			string line;
			getLineFromFile(&animationFile, &line);
			intervalTimer.changeInterval(strToInt(line));
			while (getLineFromFile(&animationFile, &line)) {
				line = pathFolder + line;
				Image *image = new Image(&line);
				animationFrames.push_back(image);
			}
			frameLoadState = FRAME_LOAD_STATE_LOADED;
		}
	}
}
	
bool Animation::getAnimationFrame(const Image **framePtr) {
	bool animationEnded = false;

	if (intervalTimer.checkTimer()) {
		if (currentFrame + 1 >= animationFrames.size()) {
			if (loop) { 
				currentFrame = 0;
			}
			animationEnded = true;
		} else {
			++currentFrame;
		}
	}

	try {
		*framePtr = animationFrames.at(currentFrame);
	} catch (out_of_range) {
		*framePtr = 0;
	}

	return animationEnded;
}

void Animation::changeAnimationInterval(uint32 interval) {
	intervalTimer.changeInterval(interval);
}

uint32 Animation::getAnimationInterval() const {
	return intervalTimer.getInterval();
}

void Animation::resetAnimation() {
	currentFrame = 0;
	intervalTimer.startTimer();
}

bool Animation::getLoop() {
	return loop;
}

void Animation::setLoop(bool loop) {
	this->loop = loop;
}

void Animation::getSize(int &width, int &height) {
	if (animationFrames.size() == 0) {
		width = 0;
		height = 0;
	} else {
		width = animationFrames[0]->getWidth();
		height = animationFrames[0]->getHeight();
	}
}

void Animation::pause() {
	intervalTimer.pauseTimer();
}

void Animation::resume() {
	intervalTimer.resumeTimer();
}

uint32 Animation::getTotalTime() {
	uint32 retVal = intervalTimer.getInterval();
	return retVal * animationFrames.size();
}

const std::vector<const Image*> *Animation::getAnimationFrames() const {
	return &animationFrames;
}

uint32 Animation::getNumberOfFrames() {
	return animationFrames.size();
}

void Animation::linkAnimationFrames(const Animation *animation) {
	const vector<const Image*> *frames = animation->getAnimationFrames();
	for (const Image *frame : *frames) {
		addFrame(frame);
	}
}