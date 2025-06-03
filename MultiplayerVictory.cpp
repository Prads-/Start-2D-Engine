#include "MultiplayerVictory.h"
#include "SfxPool.h"

MultiplayerVictory::MultiplayerVictory(StartEngine *engine, const Image *font) : Screen(engine),
		btnBackToMain(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }) {
	btnBackToMain.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBackToMain.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBackToMain.setCaption("back to main", 50, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnBackToMain.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	this->font = font;
}

void MultiplayerVictory::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnBackToMain.update(wasMouseLeftToggled);
}
	
void MultiplayerVictory::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("winner", &customFont, 16, 5, 0xFFFF00, 437, 20);
	engine->drawText("Congratulation! You've won!!!", font, 32, 32, 16, 0xFF00, 280, 100);
	btnBackToMain.draw();

	engine->presentScreen();
}