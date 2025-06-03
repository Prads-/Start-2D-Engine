#include "MultiplayerMode.h"
#include "SfxPool.h"

MultiplayerMode::MultiplayerMode(StartEngine *engine) 
				: Screen(engine),
				btnOffline(engine, Vector2i(352, 269), true, [this]() { offlineSelected(); }),
				btnJoin(engine, Vector2i(352, 429), true, [this]() { joinSelected(); }),
				btnHost(engine, Vector2i(352, 349), true, [this]() { hostSelected(); }),
				btnBack(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }) {
	btnOffline.setButtonSprite(&middleButtonBackgroundDefault);
	btnOffline.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnOffline.setCaption("offline", 97, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnOffline.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnJoin.setButtonSprite(&middleButtonBackgroundDefault);
	btnJoin.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnJoin.setCaption("join", 128, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnJoin.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnHost.setButtonSprite(&middleButtonBackgroundDefault);
	btnHost.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnHost.setCaption("host", 115, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnHost.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnBack.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBack.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBack.setCaption("back to main", 50, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnBack.setSfxMouseOver(SfxPool::getSfxButtonOver1());
}

void MultiplayerMode::offlineSelected() {
	multiplayerModeOption = MULTIPLAYER_MODE_OFFLINE;
	moveOn = MOVE_ON_NEXT;
}

void MultiplayerMode::joinSelected() {
	multiplayerModeOption = MULTIPLAYER_MODE_JOIN;
	moveOn = MOVE_ON_NEXT;
}

void MultiplayerMode::hostSelected() {
	multiplayerModeOption = MULTIPLAYER_MODE_HOST;
	moveOn = MOVE_ON_NEXT;
}

void MultiplayerMode::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnBack.update(wasMouseLeftToggled);
	btnHost.update(wasMouseLeftToggled);
	btnJoin.update(wasMouseLeftToggled);
	btnOffline.update(wasMouseLeftToggled);
}

void MultiplayerMode::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("multiplayer mode", &customFont, 16, 5, 0xFFFF00, 344, 20);

	btnOffline.draw();
	btnHost.draw();
	btnJoin.draw();
	btnBack.draw();

	engine->presentScreen();
}

void *MultiplayerMode::getUserInput() {
	return &multiplayerModeOption;
}