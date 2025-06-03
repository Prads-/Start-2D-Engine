#include "CreditScreen.h"
#include "SfxPool.h"

CreditScreen::CreditScreen(StartEngine *engine, const Image *font) : Screen(engine), 
		btnBackToMain(engine, Vector2i(0, 709), true, [this]() { moveOn = MOVE_ON_PREVIOUS; }) {
	btnBackToMain.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBackToMain.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBackToMain.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	btnBackToMain.setCaption("back to main", 30, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	this->font = font;
	transitionY = 640;
}

void CreditScreen::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnBackToMain.update(wasMouseLeftToggled);
	if (transitionY != 250) {
		transitionY -= 2;
	}
}

void CreditScreen::renderScreen() {
	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("credits", &customFont, 16, 5, 0xFFFF00, 439, 10);
	engine->drawText("developed by virtual escape games", &customFont, 16, 5, 0xAF00, 133, 125);
	engine->drawText("(www.virtualescapegames.com)", font, 32, 32, 16, 0xAF00, 288, 175);
	
	engine->drawText("Programmer:", font, 32, 32, 16, 0xFFF4C0, 311, transitionY);
	engine->drawText("- Pradipna Nepal", font, 32, 32, 16, 0xEFE4B0, 537, transitionY);
	if (transitionY + 32 < 643) {
		engine->drawText("  (i.am.prads@gmail.com)", font, 32, 32, 16, 0x9F9460, 537, transitionY + 32);
	}

	if (transitionY + 82 < 643) {
		engine->drawText("Game Design:", font, 32, 32, 16, 0xFFF4C0, 295, transitionY + 82);
		engine->drawText("- Joshua Esmero", font, 32, 32, 16, 0xEFE4B0, 537, transitionY + 82);
	}
	if (transitionY + 114 < 643) {
		engine->drawText("  (joshua.esmero@gmail.com)", font, 32, 32, 16, 0x9F9460, 537, transitionY + 114);
	}
	if (transitionY + 156 < 643) {
		engine->drawText("- Pradipna Nepal", font, 32, 32, 16, 0xEFE4B0, 537, transitionY + 156);
	}

	if (transitionY + 196 < 643) {
		engine->drawText("Graphics Design:", font, 32, 32, 16, 0xFFF4C0, 231, transitionY + 196);
		engine->drawText("- Joshua Esmero", font, 32, 32, 16, 0xEFE4B0, 537, transitionY + 196);
	}
	if (transitionY + 228 < 643) {
		engine->drawText("- Pradipna Nepal", font, 32, 32, 16, 0xEFE4B0, 537, transitionY + 228);
	}

	if (transitionY + 278 < 643) {
		engine->drawText("Music:", font, 32, 32, 16, 0xFFF4C0, 391, transitionY + 278);
		engine->drawText("- Sophie Kirschner", font, 32, 32, 16, 0xEFE4B0, 537, transitionY + 278);
	}
	if (transitionY + 310 < 643) {
		engine->drawText("  (www.pineapplemachine.com)", font, 32, 32, 16, 0x9F9460, 537, transitionY + 310);
	}
	if (transitionY + 342 < 643) {
		engine->drawText("  (pineapplemachine@gmail.com)", font, 32, 32, 16, 0x9F9460, 537, transitionY + 342);
	}

	if (transitionY + 390 < 643) {
		engine->drawText("Sound Effects:", font, 32, 32, 16, 0xFFF4C0, 263, transitionY + 390);
		engine->drawText("- Pradipna Nepal", font, 32, 32, 16, 0xEFE4B0, 537, transitionY + 390);
	}

	btnBackToMain.draw();
	engine->presentScreen();
}

void CreditScreen::resetScreen() {
	Screen::resetScreen();
	transitionY = 640;
}