#include "SurvivalModeResult.h"
#include <fstream>
#include "stringExtension.h"
using namespace std;
using namespace StringExtension;

SurvivalModeResult::SurvivalModeResult(StartEngine *engine, const Image *font, const Image *backToMainSprite) : Screen(engine), 
					btnBackToMain(engine, Vector2i(386, 350), false, [this]() { previousScreen(); }) {
    this->font = font;
	btnBackToMain.setButtonSprite(backToMainSprite);
    survivalModeResultLogo.loadImage("Images/Screens/SurvivalModeResult/SurvivalModeResult.png");
}

void SurvivalModeResult::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();

	btnBackToMain.update(wasMouseLeftToggled);
}

void SurvivalModeResult::renderScreen() {
	engine->clearScreen(0xFFFFFF);

	engine->draw(&survivalModeResultLogo, 0, false, false, false, 317, 10);
	engine->drawText("You've scored: " + intToStr(currentScore), font, 32, 32, 16, 0xFF0000, 300, 309);
	btnBackToMain.draw();

	engine->presentScreen();
}

void SurvivalModeResult::setCurrentScore(uint32 currentScore) {
	this->currentScore = currentScore;
	int highScore;
	ifstream highScoreFile("highscore.dat", ios::binary);
	if (highScoreFile.is_open()) {
		highScoreFile.read((char*)&highScore, 4);
		highScoreFile.close();
	} else {
		highScore = 0;
	}
	if (currentScore > highScore) {
		ofstream highScoreFile("highscore.dat", ios::binary);
		if (highScoreFile.is_open()) {
			highScoreFile.write((char*)&currentScore, 4);
			highScoreFile.close();
		}
	}
}
