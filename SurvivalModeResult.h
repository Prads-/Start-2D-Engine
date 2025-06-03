#ifndef SURVIVAL_MODE_RESULT_H
#define SURVIVAL_MODE_RESULT_H

#include "Screen.h"
#include "Button.h"
#include "Image.h"

class SurvivalModeResult : public Screen {
private:
	Button btnBackToMain;
	Image survivalModeResultLogo;
	const Image *font;
	uint32 currentScore;

public:
	SurvivalModeResult(StartEngine *engine, const Image *font, const Image *backToMainSprite);

	void update();
	void renderScreen();
	void setCurrentScore(uint32 currentScore);
};

#endif
