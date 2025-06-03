#ifndef SURVIVAL_MODE_H
#define SURVIVAL_MODE_H

#include "Screen.h"
#include "Button.h"
#include "Image.h"

class SurvivalMode : public Screen {
private:
	Button btnStartSurvivalMode, btnBackToMain;
	uint32 highScore, lastScore;
	const Image *font;

	void loadHighscore();
	void backToMain();

public:
	SurvivalMode(StartEngine *engine, const Image *font);

	void update();
	void renderScreen();
	void resetScreen();
	void newScore(uint32 score);
};

#endif
