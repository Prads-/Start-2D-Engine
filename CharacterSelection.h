#ifndef CHARACTER_SELECTION_H
#define CHARACTER_SELECTION_H

#include "Screen.h"
#include "Image.h"
#include "Character.h"
#include <vector>
#include "Button.h"
#include <functional>

#define CHARACTER_SELECTION_PLAYER			0
#define CHARACTER_SELECTION_OPPONENT_1		1
#define CHARACTER_SELECTION_OPPONENT_2		2
#define CHARACTER_SELECTION_OPPONENT_3		3
#define TOTAL_NUMBER_OF_OPPONENTS			4

class CharacterSelection : public Screen {
public:
	struct SelectedCharacter {
		Character *character;
		int characterPage, index;
	};

	struct CharacterPage {
		std::vector<Character> page;
	
		CharacterPage() {
			page.reserve(8);
		}
	};

private:
	std::vector<CharacterPage*> characterPages;
	int currentlySelectedCharacterPage, currentlySelectedOpponent;
	SelectedCharacter selectedOpponentCharacters[TOTAL_NUMBER_OF_OPPONENTS];
	Button *btnCharacterIcons[8], *btnCharacterPortraits[TOTAL_NUMBER_OF_OPPONENTS], btnCharacterPagePrevious, btnCharacterPageNext,
		btnPrevious, btnNext;
	Image noIcon, noPortrait, portraitSelected, tokenSprite[TOTAL_NUMBER_OF_OPPONENTS];
	Vector2i iconButtonsPosition[8];
	Image *pickupSprites;

	void getCharacterPages();
	void getEightCharacters(std::vector<Character> *characters, std::ifstream *characterList, int &id);
	void nextPage();
	void previousPage();
	void updateNextPreviousButtonsDiability();
	void updateCharacterIcons();
	Vector2i getTokenPositionFromCounterAndIndex(int counter, int index);
	void deleteSelectedCharacter();
	void updateNextScreenDisability();
	void drawShipMovesIcon(Vector2i portraitButtonPosition, const Character *character);

	void characterSelected(int index);

	void portraitSelectedPlayer();
	void portraitSelectedAI1();
	void portraitSelectedAI2();
	void portraitSelectedAI3();

public:
	CharacterSelection(StartEngine *engine, Image *pickupSprites);
	~CharacterSelection();

	const std::vector<CharacterSelection::CharacterPage*> *getCharactersList() const;

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif
