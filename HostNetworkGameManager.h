#ifndef HOST_NETWORK_GAME_MANAGER_H
#define HOST_NETWORK_GAME_MANAGER_H

#include "NetworkGameManager.h"
#include "NetworkGameSimulator.h"
#include <list>
#include "AIDifficulty.h"
#include "Button.h"
#include <functional>
#include <mutex>

class HostNetworkGameManager : public NetworkGameManager {
public:
	struct NetworkGameInformation {
		int hostCharacterId, numberOfPlayers, mapId, portNumber;
		std::string mapPath, nickname;
		bool allocateAIToEmptyPlace;
		AIDifficulty aiDifficulty;
	};

private:
	enum NetworkMode {
		MODE_INACTIVE, MODE_WAITING, MODE_SEND_BACKGROUND_DATA, MODE_DO_PRESTART, MODE_IN_GAME
	};

	struct Client {
		sockaddr_in clientAddr;
		std::string name;
		int characterId;
	};

	volatile NetworkMode mode;
	NetworkGameSimulator gameSimulator;
	std::list<Client> clients;
	std::list<Client>::iterator currentBackgroundStreamClient;
	int maxNumberOfClients;
	NetworkGameInformation gameInformation;
	Button btnCancel, btnResume, btnBackToMain;
	std::function<void()> cancelGameCallback, victoryCallback;
	int positionPacketId;
	bool hostPlayerWasVictorious;
	DataPacketServerGameStart dataPacketServerGameStart;
	uint8 *backgroundStreamBuffer;
	int streamBufferPacketId;
	FrameCountTimer streamAckWaitTimer;
	volatile bool waitingForStreamAck, backgroundStreamThreadFunctionIsRunning, backgroundStreamThreadFunctionSwitch;
	unsigned int currentStreamClientIP;
	std::mutex streamMutex;
	bool isPaused;

	void checkPacket(DataPacketDataType *packet);
	void checkPacketWhileWaiting(DataPacketDataType *packet);
	void checkPacketWhileInGame(DataPacketDataType *packet);
	void checkPacketWhileSendingBackground(DataPacketDataType *packet);

	void clientHandshakeRecieved();
	void gameInfoAckRecieved(DataPacketClientGameInfoAck *packet);
	void renderWaitingScreen();
	void renderGame();
	void cancelGame();
	void updateWaitingStatus();
	std::vector<Character> getPlayerCharacters(std::vector<int> *idListOut);
	std::vector<Character> getAICharacters(std::vector<int> *idListOut);
	void clientInputRecieved(DataPacketClientInput *packet);
	void sendPositionPacketToAllClients();
	Client *getClientFromPlayerId(unsigned int id);
	void loadLevelAssetsFileAndCreateMapBackground();
	void sendVictoryPacket();
	void sendClientsGameStartPackets();
	void createBackgroundStreamBuffer();
	void sendBackgroundStreamToClients();
	void getNextStreamClient();
	void sendBackgroundStreamDataPacket(uint16 packetId);
	void sendBackgroundStreamStartDataPacket();
	void resendStreamPacket();
	void backgroundStreamAckRecieved(DataPacketBackgroundStreamAck *packet);
	static void backgroundStreamThreadFunc(void *hostNetworkGameManager);
	void prestartStuffs();
	void renderPauseScreen();
	void resumeGame();

	void cleanUpGame();

public:
	HostNetworkGameManager(StartEngine *engine, Image *font, Image *pickupSprites, Image *pickupCross, Image *shootFrame, 
		std::function<void()> cancelGameCallback, std::function<void()> victoryCallback);
	~HostNetworkGameManager();

	bool startNetworkGame(NetworkGameInformation gameInformation);
	void render();
};

#endif
