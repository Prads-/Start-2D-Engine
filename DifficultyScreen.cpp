#include "DifficultySelection.h"
#include "SfxPool.h"

DifficultySelection::DifficultySelection(StartEngine *engine) : Screen(engine),
							btnEasy(engine, Vector2i(352, 269), true, [this]() { difficultySelected(AI_EASY); }),
							btnNormal(engine, Vector2i(352, 349), true, [this]() { difficultySelected(AI_NORMAL); }),
							btnFast(engine, Vector2i(352, 429), true, [this]() { difficultySelected(AI_FAST); }),
                            btnPrevious(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }) {
	btnEasy.setButtonSprite(&middleButtonBackgroundDefault);
	btnEasy.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnEasy.setCaption("easy", 110, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnEasy.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnNormal.setButtonSprite(&middleButtonBackgroundDefault);
	btnNormal.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnNormal.setCaption("normal", 80, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnNormal.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnFast.setButtonSprite(&middleButtonBackgroundDefault);
	btnFast.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnFast.setCaption("fast", 120, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnFast.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnPrevious.setButtonSprite(&edgeButtonBackgroundDefault);
	btnPrevious.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnPrevious.setCaption("previous", 110, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnPrevious.setSfxMouseOver(SfxPool::getSfxButtonOver1());
}

void DifficultySelection::difficultySelected(AIDifficulty difficulty) {
	this->difficulty = difficulty;
	moveOn = MOVE_ON_NEXT;
}

void DifficultySelection::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();

	btnEasy.update(wasMouseLeftToggled);
	btnNormal.update(wasMouseLeftToggled);
	btnFast.update(wasMouseLeftToggled);
	btnPrevious.update(wasMouseLeftToggled);
}

void DifficultySelection::renderScreen() {
	engine->clearScreen(0xFFFFFF);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("difficulty selection", &customFont, 16, 5, 0xFFFF00, 321, 20);

	btnEasy.draw();
	btnNormal.draw();
	btnFast.draw();
	btnPrevious.draw();

	engine->presentScreen();
}

void *DifficultySelection::getUserInput() {
	return &difficulty;
}
