#include "FrameCountTimer.h"

uint64 FrameCountTimer::totalFrames = 0;

FrameCountTimer::FrameCountTimer() { 
	interval = 0;
	previousFrames = totalFrames;
}

FrameCountTimer::FrameCountTimer(uint32 interval) {
	this->interval = interval;
	previousFrames = totalFrames;
}

void FrameCountTimer::incrementTotalFrames() {
	++totalFrames;
}

bool FrameCountTimer::checkTimer() {
	if (totalFrames - previousFrames >= interval) {
		previousFrames = totalFrames;
		return true;
	}
	return false;
}

void FrameCountTimer::startTimer() {
	previousFrames = totalFrames;
}

void FrameCountTimer::changeInterval(uint32 interval) {
	this->interval = interval;
}

uint32 FrameCountTimer::getInterval() const {
	return interval;
}

uint32 FrameCountTimer::getPassedFrames() const {
	return totalFrames - previousFrames;
}

void FrameCountTimer::pauseTimer() {
	pauseDifference = getPassedFrames();
}

void FrameCountTimer::resumeTimer() {
	previousFrames = totalFrames - pauseDifference;
}