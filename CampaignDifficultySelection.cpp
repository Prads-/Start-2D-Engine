#include "CampaignDifficultySelection.h"
#include "SfxPool.h"
#include "ConfigFile.h"

CampaignDifficultySelection::CampaignDifficultySelection(StartEngine *engine) : Screen(engine),
				btnEasy(engine, Vector2i(352, 309), true, [this]() { easyClicked(); }),
				btnHard(engine, Vector2i(352, 389), true, [this]() { hardClicked(); }),
				btnBackToMain(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }) {
	btnEasy.setButtonSprite(&middleButtonBackgroundDefault);
	btnEasy.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnEasy.setCaption("easy", 110, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnEasy.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnHard.setButtonSprite(&middleButtonBackgroundDefault);
	btnHard.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnHard.setCaption("hard", 110, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnHard.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnBackToMain.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBackToMain.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBackToMain.setCaption("back to main", 50, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnBackToMain.setSfxMouseOver(SfxPool::getSfxButtonOver1());
}

void CampaignDifficultySelection::easyClicked() {
	difficulty = EASY;
	moveOn = MOVE_ON_NEXT;
}

void CampaignDifficultySelection::hardClicked() {
	difficulty = HARD;
	moveOn = MOVE_ON_NEXT;
}

void CampaignDifficultySelection::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnEasy.update(wasMouseLeftToggled);
	btnHard.update(wasMouseLeftToggled);
	btnBackToMain.update(wasMouseLeftToggled);
}

void CampaignDifficultySelection::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("difficulty selection", &customFont, 16, 5, 0xFFFF00, 321, 20);

	btnEasy.draw();
	btnHard.draw();
	btnBackToMain.draw();

	if (hardModeLocked) {
		Vector2i position = btnHard.getPosition();
		engine->draw(&lockedSprite, 0, false, false, true, position.x + 325, position.y - 1);
	}

	engine->presentScreen();
}

void *CampaignDifficultySelection::getUserInput() {
	return &difficulty;
}

void CampaignDifficultySelection::resetScreen() {
	Screen::resetScreen();
	if (ConfigFile::getIsHardModeUnlocked()) {
		hardModeLocked = false;
		btnHard.setIsDisabled(false);
	} else {
		hardModeLocked = true;
		btnHard.setIsDisabled(true);
	}
}