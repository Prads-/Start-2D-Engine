#ifndef JOIN_GAME_H
#define JOIN_GAME_H

#include "Screen.h"
#include <string>
#include "Button.h"
#include "TextBox.h"

class JoinGame :  public Screen {
public:
	struct JoinGameInformation {
		std::string nickname;
		int port;
	};

private:
	JoinGameInformation joinGameInformation;
	Image *font;
	Button btnJoinGame, btnBack;
	TextBox txtNickname, txtPort;

	void joinGame();

public:
	JoinGame(StartEngine *engine, Image *font);

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif