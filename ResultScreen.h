#ifndef RESULT_SCREEN_H
#define RESULT_SCREEN_H

#include "Screen.h"
#include "Button.h"
#include <string>

class ResultScreen : public Screen {
private:
	Button btnBackToMain;
	std::string message;
	const Image *font;

public:
	ResultScreen(StartEngine *engine, const Image *font);

	void update();
	void renderScreen();
	void setMessage(std::string message);
};

#endif