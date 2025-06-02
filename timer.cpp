/*
	Written By Pradipna Nepal
	www.pradsprojects.com

	Copyright (C) 2013 Pradipna Nepal
	Please read COPYING.txt included along with this source code for more detail.
	If not included, see http://www.gnu.org/licenses/
*/

#include "timer.h"

#ifdef BUILD_FOR_UNIX
#include <sys/time.h>
#endif

Timer::Timer() {
	prevMilliSeconds = 0;
	pauseDifference = 0;
	this->interval = 1000;

#ifdef BUILD_FOR_WINDOWS
	timeBeginPeriod(1);
#endif
}

Timer::Timer(uint32 interval) {
	prevMilliSeconds = 0;
	pauseDifference = 0;
	this->interval = interval;

#ifdef BUILD_FOR_WINDOWS
	timeBeginPeriod(1);
#endif
}

Timer::~Timer() {
#ifdef BUILD_FOR_WINDOWS
	timeEndPeriod(1);
#endif
}

uint32 Timer::getCurrentMilliseconds() {
#ifdef BUILD_FOR_WINDOWS
	return timeGetTime();
#endif
#ifdef BUILD_FOR_UNIX
    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

bool Timer::checkTimer() {
	uint32 currentMilliSeconds;

	currentMilliSeconds = getCurrentMilliseconds();
	if (currentMilliSeconds - prevMilliSeconds >= interval) {
		prevMilliSeconds = currentMilliSeconds;
		return true;
	}

	return false;
}

void Timer::startTimer() {
	prevMilliSeconds = getCurrentMilliseconds();
}

void Timer::changeInterval(uint32 interval) {
	this->interval = interval;
}

void Timer::pauseTimer() {
	uint32 currentMilliseconds = getCurrentMilliseconds();
	pauseDifference = currentMilliseconds - prevMilliSeconds;
}

void Timer::resumeTimer() {
	uint32 currentMilliseconds = getCurrentMilliseconds();
	prevMilliSeconds = currentMilliseconds - pauseDifference;
}
