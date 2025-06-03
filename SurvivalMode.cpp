#include "SurvivalMode.h"
#include <fstream>
#include "stringExtension.h"
#include "SfxPool.h"
#include "ConfigFile.h"
using namespace std;
using namespace StringExtension;

SurvivalMode::SurvivalMode(StartEngine *engine, const Image *font) : Screen(engine), 
			btnStartSurvivalMode(engine, Vector2i(352, 349), true, [this]() { nextScreen(); }),
            btnBackToMain(engine, Vector2i(352, 431), true, [this]() { backToMain(); }) {
    btnStartSurvivalMode.setButtonSprite(&middleButtonBackgroundDefault);
	btnStartSurvivalMode.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnStartSurvivalMode.setCaption("start", 110, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnStartSurvivalMode.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	
	btnBackToMain.setButtonSprite(&middleButtonBackgroundDefault);
	btnBackToMain.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnBackToMain.setCaption("previous", 69, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnBackToMain.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	
	loadHighscore();
	this->font = font;
	lastScore = 0;
}

void SurvivalMode::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();

	btnStartSurvivalMode.update(wasMouseLeftToggled);
	btnBackToMain.update(wasMouseLeftToggled);
}

void SurvivalMode::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("survival mode", &customFont, 16, 5, 0xF2D800, 340, 20);
	if (lastScore > 0) {
		string lastScoreStr = "You scored: " + intToStr(lastScore);
		engine->drawText(lastScoreStr, font, 32, 32, 16, 0xFF00, 512 - (lastScoreStr.size() * 16) / 2, 269);
	}
	string currentHighScoreStr = "Current Highscore: " + intToStr(highScore);
	engine->drawText(currentHighScoreStr, font, 32, 32, 16, 0xFF00, 512 - (currentHighScoreStr.size() * 16)  / 2, 309);
	btnStartSurvivalMode.draw();
	btnBackToMain.draw();

	engine->presentScreen();
}

void SurvivalMode::resetScreen() {
	Screen::resetScreen();
	loadHighscore();
}

void SurvivalMode::loadHighscore() {
	highScore = ConfigFile::getSurvivalHighScore();
}

void SurvivalMode::backToMain() {
	lastScore = 0;
	previousScreen();
}

void SurvivalMode::newScore(uint32 score) {
	lastScore = score;
	if (score > highScore) {
		highScore = score;
		ConfigFile::setSurvivalHighScore(highScore);
	}
}