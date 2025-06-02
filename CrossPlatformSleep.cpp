#include "CrossPlatformSleep.h"
#include "build.h"

#ifdef BUILD_FOR_WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#endif

void CrossPlatformSleep::crossPlatformSleep(unsigned int milliseconds) {
#ifdef BUILD_FOR_WINDOWS
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}