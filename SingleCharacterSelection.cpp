#include "SingleCharacterSelection.h"
#include "SfxPool.h"

SingleCharacterSelection::SingleCharacterSelection(StartEngine *engine, const std::vector<CharacterSelection::CharacterPage*> *characterPages, const Image *pickupSprites) 
				: Screen(engine), btnNext(engine, Vector2i(679, 709), true, [this]() { nextScreen(); }),
				btnPrevious(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }),
				btnNextPage(engine, Vector2i(790, 195), false, [this]() { nextPage(); }),
				btnPreviousPage(engine, Vector2i(183, 195), false, [this]() { previousPage(); }) {
	selectedCharacter.character = &(*characterPages)[0]->page[0];
	selectedCharacter.characterPage = 0;
	selectedCharacter.index = 0;
	this->pickupSprites = pickupSprites;
	this->characterPages = characterPages;

	btnPrevious.setButtonSprite(&edgeButtonBackgroundDefault);
	btnPrevious.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnPrevious.setCaption("previous", 110, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnPrevious.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnNext.setButtonSprite(&edgeButtonBackgroundDefault);
	btnNext.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnNext.setFlipX(true);
	btnNext.setCaption("next", 100, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnNext.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnNextPage.loadButtonSprite("Images/Screens/Commons/pageSelectionNextOff.png");
	btnNextPage.loadOnMouseOverSprite("Images/Screens/Commons/pageSelectionNextOn.png");
	
	btnPreviousPage.loadButtonSprite("Images/Screens/Commons/pageSelectionPreviousOff.png");
	btnPreviousPage.loadOnMouseOverSprite("Images/Screens/Commons/pageSelectionPreviousOn.png");

	btnPreviousPage.setIsDisabled(true);
	btnNextPage.setIsDisabled(selectedCharacter.characterPage == characterPages->size() - 1);

	Vector2i iconButtonsPosition[8];

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
	updateCharacterIcons();
}

SingleCharacterSelection::~SingleCharacterSelection() {
	for (int i = 0; i < 8; ++i) {
		delete btnCharacterIcons[i];
	}
}

void SingleCharacterSelection::nextPage() {
	++selectedCharacter.characterPage;
	btnNextPage.setIsDisabled(selectedCharacter.characterPage == characterPages->size() - 1);
	updateCharacterIcons();
}

void SingleCharacterSelection::previousPage() {
	--selectedCharacter.characterPage;
	btnPreviousPage.setIsDisabled(selectedCharacter.characterPage == 0);
	updateCharacterIcons();
}

void SingleCharacterSelection::characterSelected(int index) {
	selectedCharacter.index = index;
	selectedCharacter.character = &(*characterPages)[selectedCharacter.characterPage]->page[index];
}

void SingleCharacterSelection::updateCharacterIcons() {
	int counter = 0;
	for (Character &character : (*characterPages)[selectedCharacter.characterPage]->page) {
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

void SingleCharacterSelection::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();

	for (int i = 0; i < 8; ++i) {
		btnCharacterIcons[i]->update(wasMouseLeftToggled);
	}
	btnNext.update(wasMouseLeftToggled);
	btnNextPage.update(wasMouseLeftToggled);
	btnPrevious.update(wasMouseLeftToggled);
	btnPreviousPage.update(wasMouseLeftToggled);
}

void SingleCharacterSelection::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("character selection", &customFont, 16, 5, 0xFFFF00, 301, 20);

	for (int i = 0; i < 8; ++i) {
		btnCharacterIcons[i]->draw();
	}
	btnNext.draw();
	btnNextPage.draw();
	btnPrevious.draw();
	btnPreviousPage.draw();
	engine->draw(selectedCharacter.character->getArt(), 0, false, false, true, 416, 411);

	AIVariables aiVariables = selectedCharacter.character->getAIVariables();
	engine->draw(&pickupSprites[aiVariables.specialThrow], 0, false, false, false, 476, 639);
	engine->draw(&pickupSprites[aiVariables.secondaryAbility], 0, false, false, false, 516, 639);
	
	engine->drawLine(Vector2f(416.0f, 411.0f), Vector2f(1.0f, 0.0f), 192, 0xFFFF00);
	engine->drawLine(Vector2f(416.0f, 411.0f), Vector2f(0.0f, 1.0f), 288, 0xFFFF00);
	engine->drawLine(Vector2f(416.0f, 699.0f), Vector2f(1.0f, 0.0f), 192, 0xFFFF00);
	engine->drawLine(Vector2f(608.0f, 411.0f), Vector2f(0.0f, 1.0f), 288, 0xFFFF00);

	engine->presentScreen();
}

void *SingleCharacterSelection::getUserInput() {
	return &selectedCharacter;
}