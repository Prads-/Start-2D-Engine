#include "MainScreen.h"
#include "SfxPool.h"

MainScreen::MainScreen(StartEngine *engine) 
			: Screen(engine), 
			btnSinglePlayer(engine, Vector2i(0, 323), true, [this]() { output = MAIN_SCREEN_SINGLE_PLAYER; }),
			btnMultiplayer(engine, Vector2i(0, 383), true, [this]() { output = MAIN_SCREEN_MULTIPLAYER; }),
			btnSurvivalMode(engine, Vector2i(0, 443), true, [this]() { output = MAIN_SCREEN_SURVIVAL_MODE; }),
            btnQuit(engine, Vector2i(0, 623), true, [this]() { output = MAIN_SCREEN_QUIT; }),
			btnCredits(engine, Vector2i(0, 563), true, [this]() { output = MAIN_SCREEN_CREDITS; }),
			btnOptions(engine, Vector2i(0, 503), true, [this]() { output = MAIN_SCREEN_OPTIONS; }) {
	output = MAIN_SCREEN_NONE;
	AudioCore::AudioBuffer *sfxButtonOver = SfxPool::getSfxButtonOver1();

	btnSinglePlayer.setButtonSprite(&edgeButtonBackgroundDefault);
	btnSinglePlayer.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnSinglePlayer.setSfxMouseOver(sfxButtonOver);
	btnSinglePlayer.setCaption("campaign", 85, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	btnMultiplayer.setButtonSprite(&edgeButtonBackgroundDefault);
	btnMultiplayer.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnMultiplayer.setSfxMouseOver(sfxButtonOver);
	btnMultiplayer.setCaption("multiplayer", 85, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	btnSurvivalMode.setButtonSprite(&edgeButtonBackgroundDefault);
	btnSurvivalMode.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnSurvivalMode.setSfxMouseOver(sfxButtonOver);
	btnSurvivalMode.setCaption("survival", 85, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	btnQuit.setButtonSprite(&edgeButtonBackgroundDefault);
	btnQuit.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnQuit.setSfxMouseOver(sfxButtonOver);
	btnQuit.setCaption("quit", 85, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	btnCredits.setButtonSprite(&edgeButtonBackgroundDefault);
	btnCredits.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnCredits.setSfxMouseOver(sfxButtonOver);
	btnCredits.setCaption("credits", 85, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	btnOptions.setButtonSprite(&edgeButtonBackgroundDefault);
	btnOptions.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnOptions.setSfxMouseOver(sfxButtonOver);
	btnOptions.setCaption("options", 85, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	logo.loadImage("Images/Screens/Main/Logo.png");
	mainBackground.loadImage("Images/Screens/Main/Background.png");
}

void MainScreen::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnSinglePlayer.update(wasMouseLeftToggled);
	btnMultiplayer.update(wasMouseLeftToggled);
	btnSurvivalMode.update(wasMouseLeftToggled);
	btnQuit.update(wasMouseLeftToggled);
	btnCredits.update(wasMouseLeftToggled);
	btnOptions.update(wasMouseLeftToggled);
}

void MainScreen::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&mainBackground, 0, false, false, false, 0, 0);
	engine->draw(&logo, 0, false, false, true, 80, 100);
	btnSinglePlayer.draw();
	btnMultiplayer.draw();
	btnSurvivalMode.draw();
	btnQuit.draw();
	btnCredits.draw();
	btnOptions.draw();

	engine->presentScreen();
}

void *MainScreen::getUserInput() {
	return (void*)&output;
}

void MainScreen::resetScreen() {
	output = MAIN_SCREEN_NONE;
}
