#ifndef SINGLE_CHARACTER_SELECTION_H
#define SINGLE_CHARACTER_SELECTION_H

#include "Screen.h"
#include <vector>
#include "CharacterSelection.h"

class SingleCharacterSelection : public Screen {
private:
	const std::vector<CharacterSelection::CharacterPage*> *characterPages;
	Button *btnCharacterIcons[8], btnNextPage, btnPreviousPage, btnPrevious, btnNext;
	CharacterSelection::SelectedCharacter selectedCharacter;
	Image noIcon;
	const Image *pickupSprites;

	void nextPage();
	void previousPage();
	void characterSelected(int index);
	void updateCharacterIcons();

public:
	SingleCharacterSelection(StartEngine *engine, const std::vector<CharacterSelection::CharacterPage*> *characterPages, const Image *pickupSprites);
	~SingleCharacterSelection();

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif