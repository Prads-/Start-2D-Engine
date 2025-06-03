#ifndef ARENA_SELECTION_H
#define ARENA_SELECTION_H

#include "Screen.h"
#include "Image.h"
#include "Arena.h"
#include <vector>
#include <fstream>
#include "Button.h"
#include <functional>

class ArenaSelection : public Screen {
private:
	struct ArenaPage {
		std::vector<Arena> page;

		ArenaPage() {
			page.reserve(11);
		}
	};

	std::vector<ArenaPage*> arenaPages;
	std::vector<Arena*> arenas;
	int currentlySelectedArenaPage;
	Arena *currentlySelectedArena;
	Button *btnArenaIcons[12], btnNextArenaPage, btnPreviousArenaPage, btnNext, btnPrevious;
	Image noPortraitSprite, noIconSprite, randomIconSprite;

	void loadArenaPages();
	void loadArenaPage(ArenaPage *page, std::ifstream *listFile, int &id);

	void arenaSelected(int index);
	void nextPageSelected();
	void previousPageSelected();
	void updateArenaIcons();
	void updateNextPreviousButtonsDisability();
	
public:
	ArenaSelection(StartEngine *engine);
	~ArenaSelection();

	void resetScreen();
	void update();
	void renderScreen();
	void *getUserInput();
	const std::vector<Arena*> *getArenas();
};

#endif
