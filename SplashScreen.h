#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include "Screen.h"
#include "FrameCountTimer.h"
#include "Image.h"

class SplashScreen : public Screen {
private:
	enum Mode {
		MODE_START, MODE_TIMED, MODE_END
	}mode;

	FrameCountTimer splashScreenTimer;
	Image logo;
	uint8 alphaComponent;
	bool stop;

	void startMode();
	void timedMode();
	void endMode();

public:
	SplashScreen(StartEngine *engine, int timerInterval);

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif