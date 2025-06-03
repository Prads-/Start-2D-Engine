#ifndef CHAPTER_SELECTION_H
#define CHAPTER_SELECTION_H

#include "Screen.h"
#include "Button.h"

#define NUMBER_OF_CHAPTERS					4
#define CHAPTER_SELECT_LOAD_SAVED			0

class ChapterSelection : public Screen {
private:
	int chapterIndex;
	Button *btnChapters[NUMBER_OF_CHAPTERS], btnLoadSaved, btnBack;
	bool chapterIsLocked[3];
	
	void chapterSelected(int index);

public:
	ChapterSelection(StartEngine *engine);
	~ChapterSelection();

	void update();
	void renderScreen();
	void *getUserInput();
	void resetScreen();
};

#endif