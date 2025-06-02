#ifndef HOST_GAME_H
#define HOST_GAME_H

#include "Screen.h"
#include <string>
#include "Button.h"
#include "TextBox.h"
#include "Spinner.h"
#include "CheckBox.h"

class HostGame : public Screen {
public:
	struct HostGameInformation {
		std::string nickName;
		int numberOfPlayers, port;
		bool bots;
	};

private:
	HostGameInformation hostGameInformation;
	Button btnHostGame, btnBack;
	TextBox txtNickname, txtPort;
	Spinner spnNumberOfPlayers;
	CheckBox chkBots;
	Image *font;

	void hostGame();

public:
	HostGame(StartEngine *engine, Image *font);

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif