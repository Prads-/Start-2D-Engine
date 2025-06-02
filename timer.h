/*
	Written By Pradipna Nepal
	www.pradsprojects.com

	Copyright (C) 2013 Pradipna Nepal
	Please read COPYING.txt included along with this source code for more detail.
	If not included, see http://www.gnu.org/licenses/
*/

#ifndef DESPAIR_TIMER_H
#define DESPAIR_TIMER_H

#include "declarations.h"
#include "build.h"

#ifdef BUILD_FOR_WINDOWS
#include <windows.h>
#pragma comment (lib, "Winmm.lib")
#endif

class Timer {
private:
	uint32 interval, prevMilliSeconds, pauseDifference;
	
	uint32 getCurrentMilliseconds();

public:
	Timer();
	Timer(uint32 interval);
	~Timer();

	bool checkTimer();
	void startTimer();
	void changeInterval(uint32 interval);
	void pauseTimer();
	void resumeTimer();
};

#endif
