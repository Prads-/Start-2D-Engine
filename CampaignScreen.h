#ifndef CAMPAIGN_SCREEN_H
#define CAMPAIGN_SCREEN_H

#include "Screen.h"
#include "Button.h"
#include <vector>
#include <functional>

class CampaignScreen : public Screen {
public:
	enum CampaignScreenOutput {
		CS_OUTPUT_C1_L1 = 0, CS_OUTPUT_C1_L2 = 1, CS_OUTPUT_C1_L3 = 2, CS_OUTPUT_C1_L4 = 3, CS_OUTPUT_C1_L5 = 4, CS_OUTPUT_C1_L6 = 5,
		CS_OUTPUT_C1_L7 = 6, CS_OUTPUT_C1_L8 = 7, CS_OUTPUT_C1_L9 = 8, CS_OUTPUT_C1_L10 = 9, CS_OUTPUT_C1_L11 = 10, CS_OUTPUT_C1_L12 = 11,

		CS_OUTPUT_C2_L1 = 12, CS_OUTPUT_C2_L2 = 13, CS_OUTPUT_C2_L3 = 14, CS_OUTPUT_C2_L4 = 15, CS_OUTPUT_C2_L5 = 16, CS_OUTPUT_C2_L6 = 17,
		CS_OUTPUT_C2_L7 = 18, CS_OUTPUT_C2_L8 = 19, CS_OUTPUT_C2_L9 = 20, CS_OUTPUT_C2_L10 = 21, CS_OUTPUT_C2_L11 = 22, CS_OUTPUT_C2_L12 = 23,
		
		CS_OUTPUT_C3_L1 = 24, CS_OUTPUT_C3_L2 = 25, CS_OUTPUT_C3_L3 = 26, CS_OUTPUT_C3_L4 = 27, CS_OUTPUT_C3_L5 = 28, CS_OUTPUT_C3_L6 = 29,
		CS_OUTPUT_C3_L7 = 30, CS_OUTPUT_C3_L8 = 31, CS_OUTPUT_C3_L9 = 32, CS_OUTPUT_C3_L10 = 33, CS_OUTPUT_C3_L11 = 34, CS_OUTPUT_C3_L12 = 35,
		
		CS_OUTPUT_C4_L1 = 36, CS_OUTPUT_C4_L2 = 37, CS_OUTPUT_C4_L3 = 38, CS_OUTPUT_C4_L4 = 39, CS_OUTPUT_C4_L5 = 40, CS_OUTPUT_C4_L6 = 41,
		CS_OUTPUT_C4_L7 = 42, CS_OUTPUT_C4_L8 = 43, CS_OUTPUT_C4_L9 = 44, CS_OUTPUT_C4_L10 = 45, CS_OUTPUT_C4_L11 = 46, CS_OUTPUT_C4_L12 = 47
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_CHAPTER_TRANSITION
	}mode;

	std::vector<Button> chapter1Buttons, chapter2Buttons, chapter3Buttons, chapter4Buttons;
	Button btnChapter1, btnChapter2, btnChapter3, btnChapter4;
	Button *btnChapters[4];
	int currentlySelectedChapter;
	Image campaignModeLogo, levelButtonsBackground, normalLevelSprite, miniBossLevelSprite, bossLevelSprite;
	CampaignScreenOutput output;
	Vector2i chaptersUpPosition[4], chaptersDownPosition[4], levelButtonBackgroundPosition;
	int transitionPeriodCounter;
	bool drawTransitionBackgroundFromBelow;
	std::function<void()> quitCallBack;

	void chapterClicked(int chapter);
	void chapter1Clicked();
	void chapter2Clicked();
	void chapter3Clicked();
	void chapter4Clicked();
	std::vector<Button> *getCurrentChapterLevelButtons();
	void renderTransistionPeriod();
	void pushChapterButtons();
	
public:
	CampaignScreen(StartEngine *engine, std::function<void()> quitCallBack);

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif
