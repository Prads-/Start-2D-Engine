#include "ResultScreen.h"
#include "SfxPool.h"

ResultScreen::ResultScreen(StartEngine *engine, const Image *font) : Screen(engine),
		btnBackToMain(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }) {
	this->font = font;
	btnBackToMain.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBackToMain.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBackToMain.setCaption("back to main", 50, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnBackToMain.setSfxMouseOver(SfxPool::getSfxButtonOver1());
}

void ResultScreen::update() {
	bool wasMousLeftToggled = engine->wasMouseLeftToggled();
	btnBackToMain.update(wasMousLeftToggled);
}

void ResultScreen::renderScreen() {
	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("result", &customFont, 16, 5, 0xFFFF00, 447, 20);

	engine->drawText(message, font, 32, 32, 16, 0xFF00, 512 - (message.size() * 16) / 2, 100);
	btnBackToMain.draw();

	engine->presentScreen();
}

void ResultScreen::setMessage(std::string message) {
	this->message = message;
}