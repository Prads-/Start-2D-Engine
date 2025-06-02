#include "HostGame.h"
#include "stringExtension.h"
#include "SfxPool.h"
#include "ConfigFile.h"
using namespace StringExtension;

HostGame::HostGame(StartEngine *engine, Image *font) : Screen(engine), btnBack(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }),
		btnHostGame(engine, Vector2i(352, 354), true, [this]() { hostGame(); }), 
		txtNickname(engine, Vector2i(512, 110), 250, 20, 0xFF00, 0xFFFF00, font, 0xFFFF7F, 0xFFFF00),
		txtPort(engine, Vector2i(512, 160), 100, 5, 0xFF00, 0xFFFF00, font, 0xFFFF7F, 0xFFFF00),
		spnNumberOfPlayers(engine, Vector2i(512, 210), 100, false, 2, 4, 0xFF00, 0xFFFF7F, font),
		chkBots(engine, Vector2i(512, 265), false) {
	btnHostGame.setButtonSprite(&middleButtonBackgroundDefault);
	btnHostGame.setMouseOverSprite(&middleButtonBackgroundHighlight);
	btnHostGame.setCaption("host", 115, 16, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnHostGame.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	
	btnBack.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBack.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBack.setCaption("previous", 110, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);
	btnBack.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	this->font = font;
	
	txtNickname.setText(ConfigFile::getPlayerName());
	txtPort.setText(intToStr(ConfigFile::getPortNumber()));
}

void HostGame::hostGame() {
	hostGameInformation.bots = chkBots.getIsChecked();
	hostGameInformation.nickName = txtNickname.getText();
	hostGameInformation.numberOfPlayers = spnNumberOfPlayers.getValue();
	hostGameInformation.port = strToInt(txtPort.getText());
	ConfigFile::setPlayerName(hostGameInformation.nickName, false);
	ConfigFile::setPortNumber(hostGameInformation.port);
	moveOn = MOVE_ON_NEXT;
}

void HostGame::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	txtNickname.update(wasMouseLeftToggled);
	txtPort.update(wasMouseLeftToggled);
	spnNumberOfPlayers.update(wasMouseLeftToggled);
	chkBots.update(wasMouseLeftToggled);
	btnBack.update(wasMouseLeftToggled);
	btnHostGame.update(wasMouseLeftToggled);
}

void HostGame::renderScreen() {
	engine->clearScreen(0);

	engine->draw(&background, 0, false, false, false, 0, 0);
	engine->drawText("host game", &customFont, 16, 11, 0xFFFF00, 318, 20);

	btnHostGame.draw();
	btnBack.draw();
	txtNickname.draw();
	txtPort.draw();
	spnNumberOfPlayers.draw();
	chkBots.draw();
	engine->drawText("Nickname", font, 32, 32, 16, 0xFFFF7F, 40, 110);
	engine->drawText("Port Number", font, 32, 32, 16, 0xFFFF7F, 40, 160);
	engine->drawText("Number of Players", font, 32, 32, 16, 0xFFFF7F, 40, 210);
	engine->drawText("Bots", font, 32, 32, 16, 0xFFFF7F, 40, 260);

	engine->presentScreen();
}

void *HostGame::getUserInput() {
	return &hostGameInformation;
}