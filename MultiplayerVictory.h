#ifndef MULTIPLAYER_VICTORY_H
#define MULTIPLAYER_VICTORY_H

#include "Screen.h"
#include "Button.h"

class MultiplayerVictory : public Screen {
private:
	Button btnBackToMain;
	const Image *font;

public:
	MultiplayerVictory(StartEngine *engine, const Image *font);

	void update();
	void renderScreen();
};

#endif