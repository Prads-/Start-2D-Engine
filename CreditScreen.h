#ifndef CREDIT_SCREEN_H
#define CREDIT_SCREEN_H

#include "Screen.h"
#include "Button.h"

class CreditScreen : public Screen {
private:
	Button btnBackToMain;
	const Image *font;
	int transitionY;

public:
	CreditScreen(StartEngine *engine, const Image *font);

	void update();
	void renderScreen();
	void resetScreen();
};

#endif