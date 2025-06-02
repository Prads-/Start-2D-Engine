#ifndef ANIMATION_H
#define ANIMATION_H

#include "declarations.h"
#include <vector>
#include <string>
#include "Image.h"
#include "FrameCountTimer.h"

class Animation {
private:
	enum FrameLoadState {
		FRAME_LOAD_STATE_NONE, FRAME_LOAD_STATE_LOADED, FRAME_LOAD_STATE_SET
	}frameLoadState;

	std::vector<const Image*> animationFrames;
	FrameCountTimer intervalTimer;
	int currentFrame;
	bool loop;
	
	void copyNonDynamicAttributes(const Animation &animation) {
		currentFrame = animation.currentFrame;
		loop = animation.loop;
		intervalTimer.changeInterval(animation.intervalTimer.getInterval());
	}

	void commonInit();

public:
	Animation();
	Animation(uint32 interval, bool loop = false);
	Animation(std::string path, bool loop = false);
	
	Animation(const Animation &animation) : intervalTimer(animation.intervalTimer) {
		copyNonDynamicAttributes(animation);
		
		if (frameLoadState == FRAME_LOAD_STATE_LOADED) {
			for (const Image *image : animation.animationFrames) {
				Image *newImage = new Image(*image);
				animationFrames.push_back(newImage);
			}
		} else {
			for (const Image *image : animation.animationFrames) {
				animationFrames.push_back(image);
			}
			frameLoadState = FRAME_LOAD_STATE_SET;
		}
	}

	~Animation();

	Animation &operator=(Animation animation) {
		copyNonDynamicAttributes(animation);
		if (frameLoadState == FRAME_LOAD_STATE_LOADED) {
			FrameLoadState temp = frameLoadState;
			frameLoadState = animation.frameLoadState;
			animation.frameLoadState = temp;
			animationFrames.swap(animation.animationFrames);
		} else {
			for (const Image *image : animation.animationFrames) {
				animationFrames.push_back(image);
			}
			frameLoadState = FRAME_LOAD_STATE_SET;
		}
		return *this;
	}
	
	void addFrames(std::string *paths, int numOfPaths);
	void addFrame(std::string path);
	void addFrame(const Image *frame);
	void linkAnimationFrames(const Animation *animation);
	void openAnimationFile(std::string path, bool clear = false);
	
	bool getAnimationFrame(const Image **framePtr);
	void changeAnimationInterval(uint32 interval);
	uint32 getAnimationInterval() const;
	void resetAnimation();

	bool getLoop();
	void setLoop(bool loop);

	void getSize(int &width, int &height);

	void pause();
	void resume();

	uint32 getTotalTime();
	const std::vector<const Image*> *getAnimationFrames() const;
	uint32 getNumberOfFrames();
};

#endif