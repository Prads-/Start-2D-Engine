#ifndef FRAME_COUNT_TIMER_H
#define FRAME_COUNT_TIMER_H

#include "build.h"
#include "declarations.h"

class FrameCountTimer {
private:
	static uint64 totalFrames;
	uint64 previousFrames;
	uint32 interval, pauseDifference;

public:
	FrameCountTimer();
	FrameCountTimer(uint32 interval);

	static void incrementTotalFrames();
	bool checkTimer();
	void startTimer();
	void changeInterval(uint32 interval);
	uint32 getInterval() const;
	
	uint32 getPassedFrames() const;
	
	void pauseTimer();
	void resumeTimer();
};

#endif