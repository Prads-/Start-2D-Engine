#ifndef CLIENT_NETWORK_GAME_MANAGER_H
#define CLIENT_NETWORK_GAME_MANAGER_H

#include "NetworkGameManager.h"
#include "DataPackets.h"
#include <list>
#include "DumbClientObject.h"
#include "DumbBallObject.h"
#include <string>
#include "Button.h"
#include <mutex>
#include "Immobiliser.h"

class ClientNetworkGameManager : public NetworkGameManager {
public:
	struct NetworkGameInformation {
		int characterId, portNumber;
		std::string nickname;
	};

private:
	enum Mode {
		MODE_INACTIVE, MODE_WAITING_SERVER_HANDSHAKE, MODE_WAITING_GAME_START, MODE_PRESTART, MODE_IN_GAME, MODE_CANCEL_GAME, MODE_VICTORY
	}mode;

	DataPacketServerGameInfo gameInformation;
	std::list<DumbClientObject*> opponents, allOpponents;
	DumbBallObject ball;
	int characterId;
	std::string nickname, serverName;
	Button btnResume, btnCancel, btnBackToMain;
	std::function<void()> cancelGameCallback, victoryCallback;
	Image cursor;
	std::mutex drawMutex;
	std::list<Immobiliser> immobilisers;
	DataPacketPositions::PickupPosition pickupPosition;
	int halfScreenResolutionX, halfScreenResolutionY;
	bool iAmDead, isStreamingBackground;
	uint16 streamPacketId;
	FrameCountTimer countDownTimer, broadcastPacketTimer;
	bool isCountingDown, isPaused;
	
	void checkPacket(DataPacketDataType *packet);
	void checkPacketWhileWaitingServerHandshake(DataPacketDataType *packet);
	void checkPacketWhileWaitingServerGameStart(DataPacketDataType *packet);
	void checkPacketWhileInGame(DataPacketDataType *packet);

	void gameInformationPacketRecieved(DataPacketServerGameInfo *packet);
	void gameStartPacketRecieved(DataPacketServerGameStart *packet);
	void positionPacketRecieved(DataPacketPositions *packet);
	void backgroundStreamStartPacketRecieved();
	void backgroundStreamPacketRecieved(DataPacketBackgroundStream *packet);

	void cleanUpGame();

	void renderWaitingScreen();
	void renderGame();

	void cancelGame();
	void showVictoryScreen();
	void sendInputPacket();
	void eliminateDeadOpponents();
	void doPreStartStuffs();
	void sendBroadcastPackets();
	void renderPauseScreen();
	void resumeGame();

public:
	ClientNetworkGameManager(StartEngine *engine, Image *font, Image *pickupSprites, Image *pickupCross, Image *shootFrame, 
		std::function<void()> cancelGameCallback, std::function<void()> victoryCallback);
	~ClientNetworkGameManager();

	void render();

	bool startNetworkGame(NetworkGameInformation gameInformation);
};

#endif
