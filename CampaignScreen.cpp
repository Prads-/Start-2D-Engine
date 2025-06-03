#include "CampaignScreen.h"
using namespace std;

#define TRANSITION_PERIOD_SPEED		10.0f

CampaignScreen::CampaignScreen(StartEngine *engine, std::function<void()> quitCallBack) : Screen(engine), btnChapter1(engine, Vector2i(50, 100), false, [this]() { chapter1Clicked(); }),
				btnChapter2(engine, Vector2i(50, 150), false, [this]() { chapter2Clicked(); }), 
				btnChapter3(engine, Vector2i(50, 200), false, [this]() { chapter3Clicked(); }),
                btnChapter4(engine, Vector2i(50, 250), false, [this]() { chapter4Clicked(); }) {
	currentlySelectedChapter = 5;
	mode = MODE_NORMAL;
	this->quitCallBack = quitCallBack;

	btnChapter1.loadButtonSprite("Images/Screens/SinglePlayer/chapter1Off.png");
	btnChapter2.loadButtonSprite("Images/Screens/SinglePlayer/chapter2Off.png");
	btnChapter3.loadButtonSprite("Images/Screens/SinglePlayer/chapter3Off.png");
	btnChapter4.loadButtonSprite("Images/Screens/SinglePlayer/chapter4Off.png");

	btnChapter1.loadOnMouseOverSprite("Images/Screens/SinglePlayer/chapter1On.png");
	btnChapter2.loadOnMouseOverSprite("Images/Screens/SinglePlayer/chapter2On.png");
	btnChapter3.loadOnMouseOverSprite("Images/Screens/SinglePlayer/chapter3On.png");
	btnChapter4.loadOnMouseOverSprite("Images/Screens/SinglePlayer/chapter4On.png");

	btnChapters[0] = &btnChapter1;
	btnChapters[1] = &btnChapter2;
	btnChapters[2] = &btnChapter3;
	btnChapters[3] = &btnChapter4;

	campaignModeLogo.loadImage("Images/Screens/SinglePlayer/CampaignMode.png");
	levelButtonsBackground.loadImage("Images/Screens/SinglePlayer/levelButtonsBack.png");
	normalLevelSprite.loadImage("Images/Screens/SinglePlayer/normalLevel.png");
	miniBossLevelSprite.loadImage("Images/Screens/SinglePlayer/miniBoss.png");
	bossLevelSprite.loadImage("Images/Screens/SinglePlayer/boss.png");

	chapter1Buttons.reserve(12);
	chapter2Buttons.reserve(12);
	chapter3Buttons.reserve(12);
	chapter4Buttons.reserve(12);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 3; ++j) {
			chapter1Buttons.push_back(Button(engine, Vector2i(100 + j * 290, 193 + i * 93), false, [this, i, j]() { output = (CampaignScreenOutput)(j + i * 3); }));
			chapter2Buttons.push_back(Button(engine, Vector2i(100 + j * 290, 193 + i * 93 + 50), false, [this, i, j]() { output = (CampaignScreenOutput)(j + i * 3 + 12); }));
			chapter3Buttons.push_back(Button(engine, Vector2i(100 + j * 290, 193 + i * 93 + 100), false, [this, i, j]() { output = (CampaignScreenOutput)(j + i * 3 + 24); }));
			chapter4Buttons.push_back(Button(engine, Vector2i(100 + j * 290, 193 + i * 93 + 150), false, [this, i, j]() { output = (CampaignScreenOutput)(j + i * 3 + 36); }));
		}
	}

	for (int i = 0; i < 12; ++i) {
		Image *sprite;
		if (i == 5) {
			sprite = &miniBossLevelSprite;
		} else if (i == 11) {
			sprite = &bossLevelSprite;
		} else {
			sprite = &normalLevelSprite;
		}
		chapter1Buttons[i].setButtonSprite(sprite);
		chapter2Buttons[i].setButtonSprite(sprite);
		chapter3Buttons[i].setButtonSprite(sprite);
		chapter4Buttons[i].setButtonSprite(sprite);
	}

	chaptersUpPosition[0] = Vector2i(50, 100);
	chaptersDownPosition[0] = Vector2i(50, 100);
	chaptersUpPosition[1] = Vector2i(50, 150);
	chaptersDownPosition[1] = Vector2i(50, 568);
	chaptersUpPosition[2] = Vector2i(50, 200);
	chaptersDownPosition[2] = Vector2i(50, 618);
	chaptersUpPosition[3] = Vector2i(50, 250);
	chaptersDownPosition[3] = Vector2i(50, 668);
}

void CampaignScreen::update() {
	if (mode != MODE_CHAPTER_TRANSITION) {
		bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
		vector<Button> *currentChapterLevelButtons = getCurrentChapterLevelButtons();

		if (currentChapterLevelButtons) {
			for (Button &button : *currentChapterLevelButtons) {
				button.update(wasMouseLeftToggled);
			}
		}

		btnChapter1.update(wasMouseLeftToggled);
		btnChapter2.update(wasMouseLeftToggled);
		btnChapter3.update(wasMouseLeftToggled);
		btnChapter4.update(wasMouseLeftToggled);
	}
	if (engine->isAsciiKeyPressed(KEY_ESCAPE)) {
		quitCallBack();
	}
}

void CampaignScreen::renderScreen() {
	engine->clearScreen(0xFFFFFF);

	engine->draw(&campaignModeLogo, 0, false, false, false, 335, 25);

	if (mode == MODE_CHAPTER_TRANSITION) {
		renderTransistionPeriod();
	} else {
		vector<Button> *currentChapterLevelButtons = getCurrentChapterLevelButtons();
		if (currentChapterLevelButtons) {
			engine->draw(&levelButtonsBackground, 0, false, true, false, levelButtonBackgroundPosition.x, levelButtonBackgroundPosition.y);
			for (Button &button : *currentChapterLevelButtons) {
				button.draw();
			}
		}
	}
	btnChapter1.draw();
	btnChapter2.draw();
	btnChapter3.draw();
	btnChapter4.draw();

	engine->presentScreen();
}

void *CampaignScreen::getUserInput() {
	return &output;
}

void CampaignScreen::chapterClicked(int chapter) {
	if (currentlySelectedChapter != chapter) {
		currentlySelectedChapter = chapter;
		transitionPeriodCounter = 0;
		mode = MODE_CHAPTER_TRANSITION;
	}
}

void CampaignScreen::chapter1Clicked() {
	chapterClicked(1);
	drawTransitionBackgroundFromBelow = false;
	levelButtonBackgroundPosition = Vector2i(50, 150);
}

void CampaignScreen::chapter2Clicked() {
	if (currentlySelectedChapter < 2) {
		drawTransitionBackgroundFromBelow = true;
		levelButtonBackgroundPosition = Vector2i(50, 618);
	} else {
		drawTransitionBackgroundFromBelow = false;
		levelButtonBackgroundPosition = Vector2i(50, 200);
	}
	chapterClicked(2);
}

void CampaignScreen::chapter3Clicked() {
	if (currentlySelectedChapter < 3) {
		drawTransitionBackgroundFromBelow = true;
		levelButtonBackgroundPosition = Vector2i(50, 668);
	} else {
		drawTransitionBackgroundFromBelow = false;
		levelButtonBackgroundPosition = Vector2i(50, 250);
	}
	chapterClicked(3);
}

void CampaignScreen::chapter4Clicked() {
	if (currentlySelectedChapter < 4) {
		drawTransitionBackgroundFromBelow = true;
		levelButtonBackgroundPosition = Vector2i(50, 718);
	} else {
		drawTransitionBackgroundFromBelow = false;
		levelButtonBackgroundPosition = Vector2i(50, 300);
	}
	chapterClicked(4);
}

std::vector<Button> *CampaignScreen::getCurrentChapterLevelButtons() {
	switch (currentlySelectedChapter) {
		case 1:
			return &chapter1Buttons;
		case 2:
			return &chapter2Buttons;
		case 3:
			return &chapter3Buttons;
		case 4:
			return &chapter4Buttons;
		default:
			return 0;
	}
}

void CampaignScreen::renderTransistionPeriod() {
	pushChapterButtons();

	const int TRANSITION_PERIOD_SIZE = 418;
	if (TRANSITION_PERIOD_SIZE - transitionPeriodCounter < TRANSITION_PERIOD_SPEED) {
		transitionPeriodCounter = TRANSITION_PERIOD_SIZE;
		mode = MODE_NORMAL;
		if (drawTransitionBackgroundFromBelow) {
			drawTransitionBackgroundFromBelow = false;
			levelButtonBackgroundPosition.y = levelButtonBackgroundPosition.y - transitionPeriodCounter;
		}
	} else {
		transitionPeriodCounter += TRANSITION_PERIOD_SPEED;
	}

	levelButtonsBackground.setHeight(transitionPeriodCounter);
	int y;
	if (drawTransitionBackgroundFromBelow) {
		y = levelButtonBackgroundPosition.y - transitionPeriodCounter;
	} else {
		y = levelButtonBackgroundPosition.y;
	}
	engine->draw(&levelButtonsBackground, 0, false, true, false, levelButtonBackgroundPosition.x, y);
}

void CampaignScreen::pushChapterButtons() {

	const int TOTAL_NUMBER_OF_CHAPTERS = 4;
	for (int i = currentlySelectedChapter; i < TOTAL_NUMBER_OF_CHAPTERS; ++i) {
		if (btnChapters[i]->getPosition() != chaptersDownPosition[i]) {
			if (chaptersDownPosition[i].y - btnChapters[i]->getPosition().y < TRANSITION_PERIOD_SPEED) {
				btnChapters[i]->setPosition(chaptersDownPosition[i]);
			} else {
				btnChapters[i]->setPosition(Vector2i(chaptersDownPosition[i].x, btnChapters[i]->getPosition().y + TRANSITION_PERIOD_SPEED));
			}
		}
	}
	for (int i = currentlySelectedChapter - 1; i > 0; --i) {
		if (btnChapters[i]->getPosition() != chaptersUpPosition[i]) {
			if (btnChapters[i]->getPosition().y - chaptersUpPosition[i].y < TRANSITION_PERIOD_SPEED) {
				btnChapters[i]->setPosition(chaptersUpPosition[i]);
			} else {
				btnChapters[i]->setPosition(Vector2i(chaptersUpPosition[i].x, btnChapters[i]->getPosition().y - TRANSITION_PERIOD_SPEED));
			}
		}
	}
}
