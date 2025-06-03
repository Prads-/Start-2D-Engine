#ifndef CAMPAIGN_DIFFICULTY_SELECTION_H
#define CAMPAIGN_DIFFICULTY_SELECTION_H

#include "Screen.h"
#include "Button.h"

class CampaignDifficultySelection : public Screen {
public:
	enum CampaignDifficulty {
		EASY, HARD
	};

private:
	CampaignDifficulty difficulty;
	Button btnEasy, btnHard, btnBackToMain;
	bool hardModeLocked;

	void easyClicked();
	void hardClicked();
	
public:
	CampaignDifficultySelection(StartEngine *engine);

	void update();
	void renderScreen();
	void *getUserInput();
	void resetScreen();
};

#endif