#include "ArenaSelection.h"
#include <fstream>
#include "stringExtension.h"
#include "SfxPool.h"
using namespace std;
using namespace StringExtension;

#define MAX_ARENA_IN_PAGE		12

ArenaSelection::ArenaSelection(StartEngine *engine) : Screen(engine), 
				btnNextArenaPage(engine, Vector2i(252, 657), false, [this]() { nextPageSelected(); }),
				btnPreviousArenaPage(engine, Vector2i(252, 100), false, [this]() { previousPageSelected(); }),
				btnNext(engine, Vector2i(679, 709), true, [this]() { nextScreen(); }),
				btnPrevious(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }) {
    loadArenaPages();
	currentlySelectedArena = 0;
	currentlySelectedArenaPage = 0;

	int xOffset = 0, yOffset = -123;
	for (int i = 0; i < MAX_ARENA_IN_PAGE; ++i) {
		if (i % 3 == 0) {
			xOffset = 0;
			yOffset += 123;
		} else {
			xOffset += 123;
		}
		btnArenaIcons[i] = new Button(engine, Vector2i(xOffset + 100, yOffset + 165), false, [this, i]() { arenaSelected(i); });
	}
	
	btnNextArenaPage.loadButtonSprite("Images/Screens/ArenaSelection/pageSelectionNextOff.png");
	btnNextArenaPage.loadOnMouseOverSprite("Images/Screens/ArenaSelection/pageSelectionNextOn.png");

	btnPreviousArenaPage.loadButtonSprite("Images/Screens/ArenaSelection/pageSelectionPreviousOff.png");
	btnPreviousArenaPage.loadOnMouseOverSprite("Images/Screens/ArenaSelection/pageSelectionPreviousOn.png");

	noPortraitSprite.loadImage("Images/Screens/ArenaSelection/noPortrait.png");
	noIconSprite.loadImage("Images/Screens/ArenaSelection/noIcon.png");
	
	randomIconSprite.loadImage("Images/Screens/ArenaSelection/Random.png");
	
	updateArenaIcons();
	updateNextPreviousButtonsDisability();

	btnNext.setButtonSprite(&edgeButtonBackgroundDefault);
	btnNext.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnNext.setFlipX(true);
	btnNext.setCaption("next", 100, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnNext.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnPrevious.setButtonSprite(&edgeButtonBackgroundDefault);
	btnPrevious.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnPrevious.setCaption("previous", 110, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnPrevious.setSfxMouseOver(SfxPool::getSfxButtonOver1());
}

ArenaSelection::~ArenaSelection() {
	for (ArenaPage *arenaPage : arenaPages) {
		delete arenaPage;
	}
	for (int i = 0; i < MAX_ARENA_IN_PAGE; ++i) {
		delete btnArenaIcons[i];
	}
}

void ArenaSelection::resetScreen() {
	Screen::resetScreen();
}

void ArenaSelection::loadArenaPages() {
	int id = 0;
	ifstream arenasList("maps/list.txt");
	if (arenasList.is_open()) {
		while (true) {
			ArenaPage *arenaPage = new ArenaPage;
			loadArenaPage(arenaPage, &arenasList, id);
			arenaPages.push_back(arenaPage);
			if (arenaPage->page.size() < MAX_ARENA_IN_PAGE - 1) {
				break;
			}
		}
	}
	for (ArenaPage *arenaPage : arenaPages) {
		for (Arena &arena : arenaPage->page) {
			arenas.push_back(&arena);
		}
	}
}

void ArenaSelection::loadArenaPage(ArenaPage *page, std::ifstream *listFile, int &id) {
	int counter = 0;
	string line;
	while (counter < MAX_ARENA_IN_PAGE - 1 && getLineFromFile(listFile, &line)) {
		page->page.push_back(Arena(line, id++));
		++counter;
	}
}

void ArenaSelection::arenaSelected(int index) {
	if (index == 0) {
		currentlySelectedArena = 0;
	} else {
		currentlySelectedArena = &arenaPages[currentlySelectedArenaPage]->page[index - 1];
	}
}

void ArenaSelection::nextPageSelected() {
	++currentlySelectedArenaPage;
	updateNextPreviousButtonsDisability();
}

void ArenaSelection::previousPageSelected() {
	--currentlySelectedArenaPage;
	updateNextPreviousButtonsDisability();
}

void ArenaSelection::updateArenaIcons() {
	btnArenaIcons[0]->setButtonSprite(&randomIconSprite);

	int counter = 1;
	for (Arena &arena : arenaPages[currentlySelectedArenaPage]->page) {
		btnArenaIcons[counter]->setButtonSprite(arena.getIcon());
		btnArenaIcons[counter]->setIsDisabled_NoSprite(false);
		++counter;
	}
	while (counter < MAX_ARENA_IN_PAGE) {
		btnArenaIcons[counter]->setButtonSprite(&noIconSprite);
		btnArenaIcons[counter]->setIsDisabled_NoSprite(true);
		++counter;
	}
}

void ArenaSelection::updateNextPreviousButtonsDisability() {
	btnNextArenaPage.setIsDisabled(currentlySelectedArenaPage == arenaPages.size() - 1);
	btnPreviousArenaPage.setIsDisabled(currentlySelectedArenaPage == 0);
}

void ArenaSelection::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();

	for (int i = 0; i < MAX_ARENA_IN_PAGE; ++i) {
		btnArenaIcons[i]->update(wasMouseLeftToggled);
	}
	btnNextArenaPage.update(wasMouseLeftToggled);
	btnPreviousArenaPage.update(wasMouseLeftToggled);

	btnNext.update(wasMouseLeftToggled);
	btnPrevious.update(wasMouseLeftToggled);
}

void ArenaSelection::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("arena selection", &customFont, 16, 5, 0xFFFF00, 346, 20);

	for (int i = 0; i < MAX_ARENA_IN_PAGE; ++i) {
		btnArenaIcons[i]->draw();
	}
	btnNextArenaPage.draw();
	btnPreviousArenaPage.draw();

	const Image *portrait;
	if (currentlySelectedArena) {
		portrait = currentlySelectedArena->getPortrait();
	} else {
		portrait = &noPortraitSprite;
	}
	engine->draw(portrait, 0, false, false, false, 504, 150);

	btnNext.draw();
	btnPrevious.draw();

	engine->presentScreen();
}

void *ArenaSelection::getUserInput() {
	return (void*)currentlySelectedArena;
}

const std::vector<Arena*> *ArenaSelection::getArenas() {
	return &arenas;
}
