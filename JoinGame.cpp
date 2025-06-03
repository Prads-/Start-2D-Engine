#include "JoinGame.h"
#include "stringExtension.h"
#include "SfxPool.h"
#include "ConfigFile.h"
using namespace StringExtension;

JoinGame::JoinGame(StartEngine *engine, Image *font) : Screen(engine), 
		btnBack(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }),
		btnJoinGame(engine, Vector2i(352, 334), true, [this]() { joinGame(); }),
		txtNickname(engine, Vector2i(512, 110), 250, 20, 0xFF00, 0xFFFF00, font, 0xFFFF7F, 0xFFFF00),
		txtPort(engine, Vector2i(512, 160), 100, 5, 0xFF00, 0xFFFF00, font, 0xFFFF7F, 0xFFFF00) {
	this->font = font;
	
	btnBack.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBack.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBack.setCaption("previous", 110, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnBack.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnJoinGame.setButtonSprite(&middleButtonBackgroundDefault);
	btnJoinGame.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnJoinGame.setCaption("join", 128, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnJoinGame.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	txtNickname.setText(ConfigFile::getPlayerName());
	txtPort.setText(intToStr(ConfigFile::getPortNumber()));
}

void JoinGame::joinGame() {
	joinGameInformation.nickname = txtNickname.getText();
	joinGameInformation.port = strToInt(txtPort.getText());
	ConfigFile::setPlayerName(joinGameInformation.nickname, false);
	ConfigFile::setPortNumber(joinGameInformation.port);
	moveOn = MOVE_ON_NEXT;
}

void JoinGame::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnBack.update(wasMouseLeftToggled);
	btnJoinGame.update(wasMouseLeftToggled);
	txtNickname.update(wasMouseLeftToggled);
	txtPort.update(wasMouseLeftToggled);
}

void JoinGame::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("join game", &customFont, 16, 5, 0xFFFF00, 407, 20);

	txtNickname.draw();
	txtPort.draw();
	btnJoinGame.draw();
	btnBack.draw();
	engine->drawText("Nickname", font, 32, 32, 16, 0xFFFF7F, 40, 110);
	engine->drawText("Port Number", font, 32, 32, 16, 0xFFFF7F, 40, 160);
	
	engine->presentScreen();
}

void *JoinGame::getUserInput() {
	return &joinGameInformation;
}