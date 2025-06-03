#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include "Screen.h"
#include "Button.h"

class MainScreen : public Screen {
public:
	enum MainScreenOutput {
		MAIN_SCREEN_NONE, MAIN_SCREEN_SINGLE_PLAYER, MAIN_SCREEN_MULTIPLAYER, MAIN_SCREEN_SURVIVAL_MODE, MAIN_SCREEN_QUIT, MAIN_SCREEN_CREDITS,
		MAIN_SCREEN_OPTIONS
	};

private:
	Image logo;
	Image mainBackground;
	MainScreenOutput output;
	Button btnSinglePlayer, btnMultiplayer, btnSurvivalMode, btnQuit, btnCredits, btnOptions;

public:
	MainScreen(StartEngine *engine);

	void resetScreen();

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif
