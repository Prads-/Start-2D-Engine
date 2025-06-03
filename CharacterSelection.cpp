#include "CharacterSelection.h"
#include <fstream>
#include "FileLoaderException.h"
#include "stringExtension.h"
#include "SfxPool.h"
using namespace std;
using namespace StringExtension;

CharacterSelection::CharacterSelection(StartEngine *engine, Image *pickupSprites) : Screen(engine), 
					btnCharacterPageNext(engine, Vector2i(790, 195), false, [this]() { nextPage(); }),
					btnCharacterPagePrevious(engine, Vector2i(183, 195), false, [this]() { previousPage(); }), 
					btnNext(engine, Vector2i(679, 709), true, [this]() { nextScreen(); }),
                    btnPrevious(engine, Vector2i(0, 709), true, [this]() { previousScreen(); })  {
	getCharacterPages();
	currentlySelectedCharacterPage = 0;
	currentlySelectedOpponent = 0;
	this->pickupSprites = pickupSprites;

	btnCharacterPageNext.loadButtonSprite("Images/Screens/Commons/pageSelectionNextOff.png");
	btnCharacterPageNext.loadOnMouseOverSprite("Images/Screens/Commons/pageSelectionNextOn.png");
	
	btnCharacterPagePrevious.loadButtonSprite("Images/Screens/Commons/pageSelectionPreviousOff.png");
	btnCharacterPagePrevious.loadOnMouseOverSprite("Images/Screens/Commons/pageSelectionPreviousOn.png");

	iconButtonsPosition[0] = Vector2i(258, 100);
	iconButtonsPosition[1] = Vector2i(391, 100);
	iconButtonsPosition[2] = Vector2i(524, 100);
	iconButtonsPosition[3] = Vector2i(657, 100);
	iconButtonsPosition[4] = Vector2i(258, 233);
	iconButtonsPosition[5] = Vector2i(391, 233);
	iconButtonsPosition[6] = Vector2i(524, 233);
	iconButtonsPosition[7] = Vector2i(657, 233);

	for (int i = 0; i < 8; ++i) {
		btnCharacterIcons[i] = new Button(engine, iconButtonsPosition[i], false, [this, i]() { characterSelected(i); });
	}

	noIcon.loadImage("Images/Screens/MultiplayerCharacterSelection/noIcon.png");
	noPortrait.loadImage("Images/Screens/MultiplayerCharacterSelection/noPortrait.png");
	portraitSelected.loadImage("Images/Screens/MultiplayerCharacterSelection/portraitSelected.png");
	tokenSprite[CHARACTER_SELECTION_PLAYER].loadImage("Images/Screens/MultiplayerCharacterSelection/playerToken.png");
	tokenSprite[CHARACTER_SELECTION_OPPONENT_1].loadImage("Images/Screens/MultiplayerCharacterSelection/ai1Token.png");
	tokenSprite[CHARACTER_SELECTION_OPPONENT_2].loadImage("Images/Screens/MultiplayerCharacterSelection/ai2Token.png");
	tokenSprite[CHARACTER_SELECTION_OPPONENT_3].loadImage("Images/Screens/MultiplayerCharacterSelection/ai3Token.png");
	
	for (int i = 0; i < TOTAL_NUMBER_OF_OPPONENTS; ++i) {
		selectedOpponentCharacters[i].character = 0;
	}

	updateCharacterIcons();
	updateNextPreviousButtonsDiability();

	btnCharacterPortraits[0] = new Button(engine, Vector2i(90, 411), true, [this]() { portraitSelectedPlayer(); });
	btnCharacterPortraits[1] = new Button(engine, Vector2i(307, 411), true, [this]() { portraitSelectedAI1(); });
	btnCharacterPortraits[2] = new Button(engine, Vector2i(524, 411), true, [this]() { portraitSelectedAI2(); });
	btnCharacterPortraits[3] = new Button(engine, Vector2i(741, 411), true, [this]() { portraitSelectedAI3(); });

	for (int i = 0; i < 4; ++i) {
		btnCharacterPortraits[i]->setButtonSprite(&noPortrait);
		btnCharacterPortraits[i]->setShowBorder(true);
	}
	
	btnNext.setButtonSprite(&edgeButtonBackgroundDefault);
	btnNext.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnNext.setFlipX(true);
	btnNext.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	btnNext.setCaption("next", 100, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	btnPrevious.setButtonSprite(&edgeButtonBackgroundDefault);
	btnPrevious.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnPrevious.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	btnPrevious.setCaption("previous", 110, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
}

CharacterSelection::~CharacterSelection() {
	for (CharacterPage *characterPage : characterPages) {
		delete characterPage;
	}
	for (int i = 0; i < 8; ++i) {
		delete btnCharacterIcons[i];
	}
	for (int i = 0; i < 4; ++i) {
		delete btnCharacterPortraits[i];
	}
}

void CharacterSelection::getCharacterPages() {
	int id = 0;
	ifstream characterList("characters/list.txt");
	if (characterList.is_open()) {
		while (true) {
			CharacterPage *characterPage = new CharacterPage;
			getEightCharacters(&characterPage->page, &characterList, id);
			characterPages.push_back(characterPage);
			if (characterPage->page.size() < 8) {
				break;
			}
		}
	} else {
		throw FileLoaderException("Could not open character list file");
	}
}

void CharacterSelection::getEightCharacters(std::vector<Character> *characters, std::ifstream *characterList, int &id) {
	int counter = 0;
	string line;
	while (counter < 8 && getLineFromFile(characterList, &line)) {
		characters->push_back(Character(line, id++));
		++counter;
	}
}

void CharacterSelection::nextPage() {
	++currentlySelectedCharacterPage;
	updateNextPreviousButtonsDiability();
	updateCharacterIcons();
}

void CharacterSelection::previousPage() {
	--currentlySelectedCharacterPage;
	updateNextPreviousButtonsDiability();
	updateCharacterIcons();
}

void CharacterSelection::updateNextPreviousButtonsDiability() {
	btnCharacterPagePrevious.setIsDisabled(currentlySelectedCharacterPage == 0);
	btnCharacterPageNext.setIsDisabled(currentlySelectedCharacterPage == characterPages.size() - 1);
}

void CharacterSelection::updateCharacterIcons() {
	int counter = 0;
	for (Character &character : characterPages[currentlySelectedCharacterPage]->page) {
		btnCharacterIcons[counter]->setButtonSprite(character.getAvatar());
		btnCharacterIcons[counter]->setIsDisabled_NoSprite(false);
		++counter;
	}
	while (counter < 8) {
		btnCharacterIcons[counter]->setButtonSprite(&noIcon);
		btnCharacterIcons[counter]->setIsDisabled_NoSprite(true);
		++counter;
	}
}

Vector2i CharacterSelection::getTokenPositionFromCounterAndIndex(int counter, int index) {
	return Vector2i(iconButtonsPosition[index].x + counter * (tokenSprite[0].getWidth() + 2), iconButtonsPosition[index].y + 83);
}

void CharacterSelection::deleteSelectedCharacter() {
	selectedOpponentCharacters[currentlySelectedOpponent].character = 0;
	btnCharacterPortraits[currentlySelectedOpponent]->setButtonSprite(&noPortrait);
}

void CharacterSelection::updateNextScreenDisability() {
	bool noPlayer, noAI = true;
	noPlayer = selectedOpponentCharacters[0].character == 0;
	for (int i = 1; i < 4; ++i) {
		if (selectedOpponentCharacters[i].character != 0) {
			noAI = false;
			break;
		}
	}
	btnNext.setIsDisabled(noPlayer || noAI);
}

void CharacterSelection::drawShipMovesIcon(Vector2i portraitButtonPosition, const Character *character) {
	if (character) {
		AIVariables aiVariables = character->getAIVariables();
		engine->draw(&pickupSprites[aiVariables.specialThrow], 0, false, false, false, portraitButtonPosition.x + 60, portraitButtonPosition.y + 228);
		engine->draw(&pickupSprites[aiVariables.secondaryAbility], 0, false, false, false, portraitButtonPosition.x + 100, portraitButtonPosition.y + 228);
	}
}

void CharacterSelection::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	for (int i = 0; i < 8; ++i) {
		btnCharacterIcons[i]->update(wasMouseLeftToggled);
	}
	btnCharacterPageNext.update(wasMouseLeftToggled);
	btnCharacterPagePrevious.update(wasMouseLeftToggled);
	for (int i = 0; i < 4; ++i) {
		btnCharacterPortraits[i]->update(wasMouseLeftToggled);
	}
	updateNextScreenDisability();
	btnNext.update(wasMouseLeftToggled);
	btnPrevious.update(wasMouseLeftToggled);

	if (engine->isAsciiKeyPressed(KEY_DELETE)) {
		deleteSelectedCharacter();
	}
}

void CharacterSelection::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("character selection", &customFont, 16, 5, 0xFFFF00, 300, 20);
	
	for (int i = 0; i < 8; ++i) {
		btnCharacterIcons[i]->draw();
	}
	btnCharacterPageNext.draw();
	btnCharacterPagePrevious.draw();
	for (int i = 0; i < TOTAL_NUMBER_OF_OPPONENTS; ++i) {
		btnCharacterPortraits[i]->draw();
		drawShipMovesIcon(btnCharacterPortraits[i]->getPosition(), selectedOpponentCharacters[i].character);
	}
	
	engine->drawText("player", &customFont, 16, 5, 0xFF00, 120, 365);
	engine->drawText("cpu", &customFont, 16, 5, 0xFF7F00, 365, 365);
	engine->drawText("cpu", &customFont, 16, 5, 0xFF7F00, 580, 365);
	engine->drawText("cpu", &customFont, 16, 5, 0xFF7F00, 795, 365);

	Vector2i portraitSelectedPosition;
	switch (currentlySelectedOpponent) {
		case CHARACTER_SELECTION_PLAYER:
			portraitSelectedPosition = Vector2i(90, 411);
			break;
		case CHARACTER_SELECTION_OPPONENT_1:
			portraitSelectedPosition = Vector2i(307, 411);
			break;
		case CHARACTER_SELECTION_OPPONENT_2:
			portraitSelectedPosition = Vector2i(524, 411);
			break;
		case CHARACTER_SELECTION_OPPONENT_3:
			portraitSelectedPosition = Vector2i(741, 411);
	}
	engine->draw(&portraitSelected, 0, false, false, false, portraitSelectedPosition.x, portraitSelectedPosition.y);

	for (int i = 0; i < TOTAL_NUMBER_OF_OPPONENTS; ++i) {
		if (selectedOpponentCharacters[i].character && selectedOpponentCharacters[i].characterPage == currentlySelectedCharacterPage) {
			Vector2i tokenPosition = getTokenPositionFromCounterAndIndex(selectedOpponentCharacters[i].character->getTokenCounter(), selectedOpponentCharacters[i].index);
			engine->draw(&tokenSprite[i], 0, false, false, false, tokenPosition.x, tokenPosition.y);
			selectedOpponentCharacters[i].character->incrementTokenCounter();
		}
	}
	for (int i = 0; i < TOTAL_NUMBER_OF_OPPONENTS; ++i) {
		if (selectedOpponentCharacters[i].character) {
			selectedOpponentCharacters[i].character->resetTokenCounter();
		}
	}

	btnPrevious.draw();
	btnNext.draw();

	engine->presentScreen();
}

void *CharacterSelection::getUserInput() {
	return selectedOpponentCharacters;
}

const std::vector<CharacterSelection::CharacterPage*> *CharacterSelection::getCharactersList() const {
	return &characterPages;
}

void CharacterSelection::characterSelected(int index) {
	selectedOpponentCharacters[currentlySelectedOpponent].character = &characterPages[currentlySelectedCharacterPage]->page[index];
	selectedOpponentCharacters[currentlySelectedOpponent].characterPage = currentlySelectedCharacterPage;
	selectedOpponentCharacters[currentlySelectedOpponent].index = index;
	btnCharacterPortraits[currentlySelectedOpponent]->setButtonSprite(selectedOpponentCharacters[currentlySelectedOpponent].character->getArt());
	++currentlySelectedOpponent;
	if (currentlySelectedOpponent == TOTAL_NUMBER_OF_OPPONENTS) {
		currentlySelectedOpponent = 0;
	}
}

void CharacterSelection::portraitSelectedPlayer() {
	currentlySelectedOpponent = 0;
}

void CharacterSelection::portraitSelectedAI1() {
	currentlySelectedOpponent = 1;
}

void CharacterSelection::portraitSelectedAI2() {
	currentlySelectedOpponent = 2;
}

void CharacterSelection::portraitSelectedAI3() {
	currentlySelectedOpponent = 3;
}
