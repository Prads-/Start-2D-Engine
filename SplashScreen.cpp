#include "SplashScreen.h"

SplashScreen::SplashScreen(StartEngine *engine, int timerInterval) : Screen(engine), splashScreenTimer(timerInterval) { 
	logo.loadImage("Images/logo.png");
	alphaComponent = 0;
	logo.modifyAlphaComponent(0);
	mode = MODE_START;
	stop = false;
}

void SplashScreen::update() {
	switch (mode) {
		case MODE_START:
			startMode();
			break;
		case MODE_TIMED:
			timedMode();
			break;
		default:
			endMode();
	}
}

void SplashScreen::renderScreen() {
	engine->clearScreen(0);
	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->draw(&logo, 0, false, false, true, 100, 335);
	engine->presentScreen();
}

void *SplashScreen::getUserInput() {
	return &stop;
}

void SplashScreen::startMode() {
	const int ALPHA_STRIDE = 2;

	if (alphaComponent + ALPHA_STRIDE >= 255) {
		alphaComponent = 255;
		mode = MODE_TIMED;
		splashScreenTimer.startTimer();
	} else {
		alphaComponent += ALPHA_STRIDE;
	}

	logo.modifyAlphaComponent(alphaComponent);
}

void SplashScreen::timedMode() {
	if (splashScreenTimer.checkTimer()) {
		mode = MODE_END;
	}
}

void SplashScreen::endMode() {
	const int ALPHA_STRIDE = 2;
	if (alphaComponent - ALPHA_STRIDE <= 0) {
		alphaComponent = 0;
		stop = true;
	} else {
		alphaComponent -= ALPHA_STRIDE;
	}

	logo.modifyAlphaComponent(alphaComponent);
}