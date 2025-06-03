#ifndef MULTIPLAYER_MODE_H
#define MULTIPLAYER_MODE_H

#include "Screen.h"
#include "Button.h"

class MultiplayerMode : public Screen {
public:
	enum MultiplayerModeOption {
		MULTIPLAYER_MODE_OFFLINE, MULTIPLAYER_MODE_HOST, MULTIPLAYER_MODE_JOIN
	};

private:
	MultiplayerModeOption multiplayerModeOption;
	Button btnOffline, btnHost, btnJoin, btnBack;
	
	void offlineSelected();
	void joinSelected();
	void hostSelected();

public:
	MultiplayerMode(StartEngine *engine);

	void update();
	void renderScreen();
	void *getUserInput();
};

#endif