#include "ChapterSelection.h"
#include "SfxPool.h"
#include "ConfigFile.h"

ChapterSelection::ChapterSelection(StartEngine *engine) : Screen(engine), 
		btnLoadSaved(engine, Vector2i(352, 509), true, [this]() { chapterSelected(CHAPTER_SELECT_LOAD_SAVED); }),
		btnBack(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }) {
	for (int i = 0; i < NUMBER_OF_CHAPTERS; ++i) {
		btnChapters[i] = new Button(engine, Vector2i(352, 189 + 80 * i), true, [this, i]() { chapterSelected(i + 1); });
		btnChapters[i]->setButtonSprite(&middleButtonBackgroundDefault);
		btnChapters[i]->setMouseOverSprite(&middleButtonBackgroundHighlight);
		btnChapters[i]->setSfxMouseOver(SfxPool::getSfxButtonOver1());
	}
	btnChapters[0]->setCaption("chapter one", 40, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnChapters[1]->setCaption("chapter two", 36, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnChapters[2]->setCaption("chapter three", 17, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnChapters[3]->setCaption("chapter four", 33, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	btnLoadSaved.setButtonSprite(&middleButtonBackgroundDefault);
	btnLoadSaved.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnLoadSaved.setCaption("load saved", 50, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnLoadSaved.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnBack.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBack.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBack.setCaption("previous", 110, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnBack.setSfxMouseOver(SfxPool::getSfxButtonOver1());
}

ChapterSelection::~ChapterSelection() {
	for (int i = 0; i < NUMBER_OF_CHAPTERS; ++i) {
		delete btnChapters[i];
	}
}

void ChapterSelection::chapterSelected(int index) {
	chapterIndex = index;
	moveOn = MOVE_ON_NEXT;
}

void ChapterSelection::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	for (int i = 0; i < NUMBER_OF_CHAPTERS; ++i) {
		btnChapters[i]->update(wasMouseLeftToggled);
	}
	btnLoadSaved.update(wasMouseLeftToggled);
	btnBack.update(wasMouseLeftToggled);
}

void ChapterSelection::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("chapter selection", &customFont, 16, 5, 0xFFFF00, 325, 20);

	for (int i = 0; i < NUMBER_OF_CHAPTERS; ++i) {
		btnChapters[i]->draw();
	}
	btnLoadSaved.draw();
	btnBack.draw();

	for (int i = 0; i < 3; ++i) {
		if (chapterIsLocked[i]) {
			Vector2i position = btnChapters[i + 1]->getPosition();
			engine->draw(&lockedSprite, 0, false, false, true, position.x + 325, position.y - 1);
		}
	}

	engine->presentScreen();
}

void *ChapterSelection::getUserInput() {
	return &chapterIndex;
}

void ChapterSelection::resetScreen() {
	Screen::resetScreen();

	for (int i = 0; i < 3; ++i) {
		if (ConfigFile::getIsChapterUnlocked(i + 1)) {
			chapterIsLocked[i] = false;
			btnChapters[i + 1]->setIsDisabled(false);
		} else {
			chapterIsLocked[i] = true;
			btnChapters[i + 1]->setIsDisabled(true);
		}
	}
}