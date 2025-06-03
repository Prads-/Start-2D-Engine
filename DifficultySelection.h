#ifndef DIFFICULTY_SELECTION_H
#define DIFFICULTY_SELECTION_H

#include "Screen.h"
#include "Button.h"
#include "Competitor.h"

class DifficultySelection : public Screen {
private:
	Button btnEasy, btnNormal, btnFast, btnPrevious;
	AIDifficulty difficulty;
	
	void difficultySelected(AIDifficulty difficulty);

public:
	DifficultySelection(StartEngine *engine);

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif
