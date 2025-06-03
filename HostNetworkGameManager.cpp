#include "HostNetworkGameManager.h"
#include "stringExtension.h"
#include <fstream>
#include "MT_RandomGenerator.h"
#include "FileLoader.h"
#include "Explosion.h"
#include "Screen.h"
#include "SfxPool.h"
#include "CrossPlatformSleep.h"
#include "GameManager.h"
#ifdef BUILD_FOR_WINDOWS
#include <process.h>
#endif
using namespace std;
using namespace StringExtension;
using namespace FileLoader;
using namespace CrossPlatformSleep;

#define STREAM_PACKET_MAX_ID				((1024 * 768 * 3) / DATA_PACKET_STREAM_DATA_SIZE)

HostNetworkGameManager::HostNetworkGameManager(StartEngine *engine, Image *font, Image *pickupSprites, Image *pickupCross, 
											   Image *shootFrame, function<void()> cancelGameCallback, std::function<void()> victoryCallback) : 
						NetworkGameManager(engine, font, pickupSprites, shootFrame, pickupCross), gameSimulator(engine), 
						btnCancel(engine, Vector2i(0, 709), true, [this]() { cancelGame(); }),
						btnResume(engine, Vector2i(352, 250), true, [this]() { resumeGame(); }),
						btnBackToMain(engine, Vector2i(352, 332), true, [this]() { cancelGame(); }) {
	btnCancel.setButtonSprite(Screen::getEdgeButtonBackgroundDefault());
	btnCancel.setMouseOverSprite(Screen::getEdgeButtonBackgroundHighlight());
	btnCancel.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	btnCancel.setCaption("back to main", 50, 5, 0, Screen::getCustomFont(), 0xF2D8D8, 0xEDBDA3);

	btnResume.setButtonSprite(Screen::getMiddleButtonBackgroundDefault2());
	btnResume.setMouseOverSprite(Screen::getMiddleButtonBackgroundHighlight());
	btnResume.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	btnResume.setCaption("resume", 89, 16, 0, Screen::getCustomFont(), 0xF2D800, 0xEDBD00);

	btnBackToMain.setButtonSprite(Screen::getMiddleButtonBackgroundDefault2());
	btnBackToMain.setMouseOverSprite(Screen::getMiddleButtonBackgroundHighlight());
	btnBackToMain.setCaption("back to main", 24, 16, 0, Screen::getCustomFont(), 0xF2D800, 0xEDBD00);
	btnBackToMain.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	mode = MODE_INACTIVE;
	this->cancelGameCallback = cancelGameCallback;
	this->victoryCallback = victoryCallback;
	backgroundStreamBuffer = 0;
	streamAckWaitTimer.changeInterval(180);
	currentStreamClientIP = 0;
	backgroundStreamThreadFunctionIsRunning = false;
	backgroundStreamThreadFunctionSwitch = false;
	isPaused = false;
}

HostNetworkGameManager::~HostNetworkGameManager() {
	cleanUpGame();
}

void HostNetworkGameManager::checkPacket(DataPacketDataType *packet) {
	switch (mode) {
		case MODE_WAITING:
			checkPacketWhileWaiting(packet);
			break;
		case MODE_IN_GAME:
			checkPacketWhileInGame(packet);
			break;
		case MODE_SEND_BACKGROUND_DATA:
			checkPacketWhileSendingBackground(packet);
	}
}

void HostNetworkGameManager::checkPacketWhileWaiting(DataPacketDataType *packet) {
	switch (packet->dpType) {
		case DPTYPE_CLIENT_HANDSHAKE:
			clientHandshakeRecieved();
			break;
		case DPTYPE_CLIENT_GAME_INFO_ACK:
			gameInfoAckRecieved((DataPacketClientGameInfoAck*)packet);
			break;
	}
}

void HostNetworkGameManager::checkPacketWhileInGame(DataPacketDataType *packet) {
	if (packet->dpType == DPTYPE_CLIENT_INPUT) {
		clientInputRecieved((DataPacketClientInput*)packet);
	}
}

void HostNetworkGameManager::checkPacketWhileSendingBackground(DataPacketDataType *packet) {
	if (packet->dpType == DPTYPE_CLIENT_BACKGROUND_STREAM_ACK) {
		backgroundStreamAckRecieved((DataPacketBackgroundStreamAck*)packet);
	}
}

void HostNetworkGameManager::clientHandshakeRecieved() {
	if (clients.size() < maxNumberOfClients) {
		sockaddr_in clientAddr = engine->getClientAddress();
		DataPacketServerGameInfo gameInfoPacket;
		gameInfoPacket.hasBots = gameInformation.allocateAIToEmptyPlace;
		gameInfoPacket.mapId = gameInformation.mapId;
		gameInfoPacket.numberOfPlayer = gameInformation.numberOfPlayers;
		
		for (int i = 0; i < 20; ++i) {
			if (gameInformation.nickname.size() > i) {
				gameInfoPacket.serverName[i] = gameInformation.nickname[i];
			} else {
				gameInfoPacket.serverName[i] = 0;
			}
		}
		
		engine->sendDataPacket((uint8*)&gameInfoPacket, sizeof(DataPacketServerGameInfo), clientAddr);
	}
}

void HostNetworkGameManager::gameInfoAckRecieved(DataPacketClientGameInfoAck *packet) {
	if (clients.size() < maxNumberOfClients) {
		sockaddr_in clientAddr = engine->getClientAddress();
		for (const Client &client : clients) {
			if (clientAddr.sin_addr.s_addr == client.clientAddr.sin_addr.s_addr) {
				return;
			}
		}
		Client client;
		client.clientAddr = engine->getClientAddress();
		int counter = 0;
		while (packet->name[counter] != 0 && counter < 19) {
			client.name.push_back(packet->name[counter++]);
		}
		client.characterId = packet->characterId;
		clients.push_back(client);
	}
}

bool HostNetworkGameManager::startNetworkGame(NetworkGameInformation gameInformation) {
	cleanUpGame();
	engine->setGridOffsets(0, 0);
	positionPacketId = 1;
	this->gameInformation = gameInformation;
	maxNumberOfClients = gameInformation.numberOfPlayers;
	if (engine->initialiseServerNetwork(gameInformation.portNumber, [this](uint8 *packet) { packetRecieved(packet); })) {
		mode = MODE_WAITING;
		return true;
	}
	return false;
}

void HostNetworkGameManager::render() {
	switch (mode) {
		case MODE_IN_GAME:
			renderGame();
			break;
		case MODE_DO_PRESTART:
			prestartStuffs();
			break;
		default:
			renderWaitingScreen();
	}
}

void HostNetworkGameManager::renderWaitingScreen() {
	engine->clearScreen(0);

	engine->draw(Screen::getBackground(), 0, false, false, false, 0, 0);

	engine->drawText("waiting for players", Screen::getCustomFont(), 16, 5, 0xFFFF00, 303, 20);
	engine->drawText("Players joined for this match (" + intToStr(clients.size() + 1) + " of " + intToStr(maxNumberOfClients) + "):", font, 32, 32, 16, 0xFF00, 40, 100);
	int counter = 1;
	engine->drawText("- " + gameInformation.nickname, font, 32, 32, 16, 0xFF00, 50, 132);
	for (const Client &client : clients) {
		engine->drawText("- " + client.name, font, 32, 32, 16, 0xFF00, 50, 132 + counter * 32);
		++counter;
	}
	btnCancel.draw();
	
	streamMutex.lock();
	if (mode == MODE_SEND_BACKGROUND_DATA) {
		engine->drawText("Sending background data to " + currentBackgroundStreamClient->name + " - " + 
			floatToString(((float)streamBufferPacketId / DATA_PACKET_NUMBER_OF_STREAM_PACKET) * 100.0f, 0) + "%", font, 32, 32, 16, 0xFF00, 40, 260);
	} else if (mode == MODE_WAITING) {
		updateWaitingStatus();
	}
	streamMutex.unlock();

	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnCancel.update(wasMouseLeftToggled);

	engine->presentScreen();
}

void HostNetworkGameManager::renderGame() {
	gameSimulator.simulateAFrame(isPaused);	
	sendPositionPacketToAllClients();
	
	list<Competitor*> *competitors = gameSimulator.getCompetitors();
	if (competitors->size() > 1) {
		engine->clearScreen();

		PlayerObject *hostPlayer = gameSimulator.getHostPlayer();
		if (!hostPlayer->isDead()) {
			hostPlayer->presentHUD();
			if (hostPlayer->hasBall()) {
				engine->draw(shootFrame, 0, false, false, true, 287, 0);
			}
			hostPlayer->drawCursor();
		}
		for (Competitor *competitor : *competitors) {
			competitor->draw();
		}

		gameSimulator.getBall()->draw();
		list<Immobiliser> *immobilisers = gameSimulator.getImmobilisers();
		for (Immobiliser &immobiliser : *immobilisers) {
			immobiliser.draw();
		}
		Explosion *explosion = Explosion::getInstance();
		if (explosion->getIsActive()) {
			explosion->update();
			explosion->draw();
		}
		PickupItem *pickupItem = PickupItem::getInstance();
		if (pickupItem->getIsActive()) {
			pickupItem->draw();
		} else {
			pickupItem->update();
		}

		if (isPaused) {
			renderPauseScreen();
		} else if (gameSimulator.getIsCountingDown()) {
			engine->draw(GameManager::getReady(), 0, false, false, true, 212, 200);	
		}

		engine->presentScreen();
	
		if (engine->isAsciiKeyPressed(KEY_ESCAPE)) {
			engine->showCursor(true);
			isPaused = true;
		}
	} else {
		sendVictoryPacket();
		cancelGame();
	}
}

void HostNetworkGameManager::renderPauseScreen() {
	engine->makeFrameMonochrome();
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnBackToMain.update(wasMouseLeftToggled);
	btnResume.update(wasMouseLeftToggled);

	btnBackToMain.draw();
	btnResume.draw();
}

void HostNetworkGameManager::resumeGame() {
	engine->showCursor(false);
	isPaused = false;
}

void HostNetworkGameManager::cancelGame() {
	DataPacketCancelGame cancelGamePacket(true);
	for (const Client &client : clients) {
		engine->sendDataPacket((uint8*)&cancelGamePacket, sizeof(cancelGamePacket), client.clientAddr);
	}
	if (hostPlayerWasVictorious) {
		victoryCallback();
	} else {
		cancelGameCallback();
	}
	cleanUpGame();
}

void HostNetworkGameManager::updateWaitingStatus() {
	if (clients.size() == maxNumberOfClients - 1) {
		vector<int> characterIds;
		vector<Character> playerCharacters = getPlayerCharacters(&characterIds);
		vector<Character> aiCharacters = getAICharacters(&characterIds);
		gameSimulator.startNetworkGame(&playerCharacters, &aiCharacters, gameInformation.mapPath, gameInformation.aiDifficulty, pickupSprites, 
			font, pickupCross);
		
		int counter = 0;
		for (int &id : characterIds) {
			dataPacketServerGameStart.characterIds[counter++] = id;
		}

		if (gameInformation.mapId >= 0) {
			loadLevelAssetsFileAndCreateMapBackground();
			mode = MODE_DO_PRESTART;
		} else {
			createBackgroundStreamBuffer();
			currentBackgroundStreamClient = clients.begin();
			streamBufferPacketId = 0;
			currentStreamClientIP = currentBackgroundStreamClient->clientAddr.sin_addr.s_addr;
			mode = MODE_SEND_BACKGROUND_DATA;
			sendBackgroundStreamStartDataPacket();
#ifdef BUILD_FOR_WINDOWS
			_beginthread(backgroundStreamThreadFunc, 0, this);
#endif
		}
	}
}

void HostNetworkGameManager::sendVictoryPacket() {
	vector<PlayerObject*> *players = gameSimulator.getPlayerObjects();
	if (!(*players)[0]->isDead()) {
		hostPlayerWasVictorious = true;
	} else {
		NetworkPlayerObject *victoriousPlayer = 0;
		for (int i = 1; i < players->size(); ++i) {
			if (!(*players)[i]->isDead()) {
				victoriousPlayer = (NetworkPlayerObject*)(*players)[i];
				break;
			}
		}
		if (victoriousPlayer) {
			unsigned int playerId = victoriousPlayer->getId();
			for (list<Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
				if (it->clientAddr.sin_addr.s_addr == playerId) {
					DataPacketVictoryPacket victoryPacket;
					engine->sendDataPacket((uint8*)&victoryPacket, sizeof(DataPacketVictoryPacket), it->clientAddr);
					clients.erase(it);
					break;
				}
			}
		}
	}
}

void HostNetworkGameManager::sendClientsGameStartPackets() {
	const vector<PlayerObject*> *playersList = gameSimulator.getPlayerObjects();
	list<Client>::iterator clientIt = clients.begin();
	for (size_t i = 1; i < playersList->size(); ++i) {
		((NetworkPlayerObject*)(*playersList)[i])->setId(clientIt->clientAddr.sin_addr.s_addr);
		engine->sendDataPacket((uint8*)&dataPacketServerGameStart, sizeof(DataPacketServerGameStart), clientIt->clientAddr);
		++clientIt;
	}
}

void HostNetworkGameManager::createBackgroundStreamBuffer() {
	const uint8 *backgroundBuffer = (uint8*)engine->getBackgroundBuffer();
	backgroundStreamBuffer = new uint8[1024 * 768 * 3];
	
	int streamBufferCounter = 0;
	for (int i = 0; i < 1024 * 768 * 4; ++i) {
		backgroundStreamBuffer[streamBufferCounter++] = backgroundBuffer[i++];
		backgroundStreamBuffer[streamBufferCounter++] = backgroundBuffer[i++];
		backgroundStreamBuffer[streamBufferCounter++] = backgroundBuffer[i++];
	}
}

void HostNetworkGameManager::sendBackgroundStreamToClients() {
	backgroundStreamThreadFunctionSwitch = true;
	backgroundStreamThreadFunctionIsRunning = true;
	while (backgroundStreamThreadFunctionSwitch) {
		streamMutex.lock();
		if (waitingForStreamAck) {
			if (streamAckWaitTimer.checkTimer()) {
				resendStreamPacket();
			}
		} else {
			if (streamBufferPacketId < STREAM_PACKET_MAX_ID) {
				sendBackgroundStreamDataPacket(streamBufferPacketId++);
			} else {
				getNextStreamClient();
			}
		}
		streamMutex.unlock();
	}
	backgroundStreamThreadFunctionIsRunning = false;
}

void HostNetworkGameManager::sendBackgroundStreamDataPacket(uint16 packetId) {
	DataPacketBackgroundStream backgroundStream;
	backgroundStream.packetId = packetId;
	memcpy(backgroundStream.backgroundData, &backgroundStreamBuffer[packetId * DATA_PACKET_STREAM_DATA_SIZE], DATA_PACKET_STREAM_DATA_SIZE);
	engine->sendDataPacket((uint8*)&backgroundStream, sizeof(DataPacketBackgroundStream), currentBackgroundStreamClient->clientAddr);
	waitingForStreamAck = true;
	streamAckWaitTimer.startTimer();
}

void HostNetworkGameManager::sendBackgroundStreamStartDataPacket() {
	DataPacketBackgroundStreamStart backgroundStreamStart;
	engine->sendDataPacket((uint8*)&backgroundStreamStart, sizeof(DataPacketBackgroundStreamStart), currentBackgroundStreamClient->clientAddr);
	waitingForStreamAck = true;
	streamAckWaitTimer.startTimer();
}

void HostNetworkGameManager::resendStreamPacket() {
	if (streamBufferPacketId == 0) {
		sendBackgroundStreamStartDataPacket();
	} else {
		sendBackgroundStreamDataPacket(streamBufferPacketId - 1);
	}
}

void HostNetworkGameManager::backgroundStreamAckRecieved(DataPacketBackgroundStreamAck *packet) {
	streamMutex.lock();
	if (packet->packetId == (uint16)(streamBufferPacketId - 1)) {
		sockaddr_in clientAddress = engine->getClientAddress();
		if (clientAddress.sin_addr.s_addr == currentStreamClientIP) {
			waitingForStreamAck = false;
		}
	}
	streamMutex.unlock();
}

void HostNetworkGameManager::backgroundStreamThreadFunc(void *hostNetworkGameManager) {
	((HostNetworkGameManager*)hostNetworkGameManager)->sendBackgroundStreamToClients();
}

void HostNetworkGameManager::prestartStuffs() {
	sendClientsGameStartPackets();
	const vector<PlayerObject*> *playersList = gameSimulator.getPlayerObjects();
	(*playersList)[0]->renderHUD();
	engine->setRenderHUD(true);
	gameSimulator.startCountdown();
	engine->showCursor(false);
	mode = MODE_IN_GAME;
}

void HostNetworkGameManager::getNextStreamClient() {
	streamBufferPacketId = 0;
	++currentBackgroundStreamClient;
	if (currentBackgroundStreamClient == clients.end()) {
		if (backgroundStreamBuffer) {
			delete [] backgroundStreamBuffer;
			backgroundStreamBuffer = 0;
		}
		mode = MODE_DO_PRESTART;
		backgroundStreamThreadFunctionSwitch = false;
	} else {
		currentStreamClientIP = currentBackgroundStreamClient->clientAddr.sin_addr.s_addr;
		sendBackgroundStreamStartDataPacket();
	}
}

void HostNetworkGameManager::loadLevelAssetsFileAndCreateMapBackground() {
	engine->createBackground(gameInformation.mapPath, &prioritisedAssets, &prioritisedImageCollector);
}

std::vector<Character> HostNetworkGameManager::getPlayerCharacters(std::vector<int> *idListOut) {
	vector<string> characterList;
	loadCharacterList(&characterList);
	
	vector<Character> retVal;
	string characterPath =  characterList[gameInformation.hostCharacterId];
	retVal.push_back(Character(characterPath, false));
	idListOut->push_back(gameInformation.hostCharacterId);
	for (const Client &client : clients) {
		try {
			string characterPath = characterList.at(client.characterId);
			retVal.push_back(Character(characterPath, false));
			idListOut->push_back(client.characterId);
		} catch (out_of_range) { }
	}

	return retVal;
}

std::vector<Character> HostNetworkGameManager::getAICharacters(std::vector<int> *idListOut) {
	vector<Character> retVal;

	if (gameInformation.allocateAIToEmptyPlace) {
		int numOfAIs = 3 - clients.size();
		if (numOfAIs > 0) {
			vector<string> characterList;
			loadCharacterList(&characterList);

			while (numOfAIs != 0) {
				int randomIndex = MT_RandomGenerator::randomNumber() % characterList.size();
				retVal.push_back(Character(characterList.at(randomIndex), false));
				--numOfAIs;
				idListOut->push_back(randomIndex);
			}
		}
	}

	return retVal;
}

void HostNetworkGameManager::clientInputRecieved(DataPacketClientInput *packet) {
	sockaddr_in clientAddr = engine->getClientAddress();
	vector<PlayerObject*> *playersList = gameSimulator.getPlayerObjects();
	for (int i = 1; i < playersList->size(); ++i) {
		if (((NetworkPlayerObject*)(*playersList)[i])->getId() == clientAddr.sin_addr.s_addr) {
			((NetworkPlayerObject*)(*playersList)[i])->setDataPacketClientInput(*packet);
			break;
		}
	}
}

void HostNetworkGameManager::sendPositionPacketToAllClients() {
	BallObject *ball = gameSimulator.getBall();
	DataPacketPositions positionPacket;
	
	positionPacket.dpId = positionPacketId++;
	positionPacket.ballPosition.x = ball->getPosition().x;
	positionPacket.ballPosition.y = ball->getPosition().y;
	positionPacket.ballPosition.flags = ball->getHitFlag();
	if (ball->taken()) {
		positionPacket.ballPosition.flags |= SERVER_BALL_IS_TAKEN;
	} else if (ball->getSpeed() <= BALL_CATCHABLE_SPEED) {
		positionPacket.ballPosition.flags |= SERVER_BALL_IS_CATCHABLE;
	}
	if (ball->getHasExploded()) {
		positionPacket.ballPosition.flags |= SERVER_BALL_EXPLODE;
	}

	vector<Competitor*> *competitors = gameSimulator.getAllCompetitors();
	int counter = 0;
	PickupItem *pickupItem = PickupItem::getInstance();
	for (const Competitor *competitor : *competitors) {
		DataPacketPositions::OpponentPositionData *positionData = &positionPacket.playersPosition[counter++];
		positionData->x = competitor->getPosition().x;
		positionData->y = competitor->getPosition().y;
		positionData->flags = competitor->getShieldActive();
		positionData->rotateDegree = competitor->getRotateDegree();
		if (competitor->getPlayDamageAnimation()) {
			positionData->flags |= SERVER_PLAYER_IS_DAMAGED;
		}
		if (competitor->hasBall()) {
			positionData->flags |= SERVER_PLAYER_HAS_BALL;
		}
		if (competitor->isInDeathPeriod()) {
			positionData->flags |= SERVER_PLAYER_IS_DEAD;
		}
		if (competitor->getShotImmobiliser()) {
			positionData->flags |= SERVER_PLAYER_HAS_SHOT_STUN;
		}
		if (competitor->getShotBall()) {
			positionData->flags |= SERVER_PLAYER_HAS_SHOT_BALL;
		}
		if (competitor->getIsImmobilised()) {
			positionData->flags |= SERVER_PLAYER_IS_STUNNED;
		}
		if (competitor->getShieldActive()) {
			positionData->flags |= SERVER_PLAYER_IS_SHIELDED;
		}
	}
	if (pickupItem->getIsActive()) {
		positionPacket.pickupPosition.x = (int)pickupItem->getPosition().x | 0x8000;
		positionPacket.pickupPosition.y = pickupItem->getPosition().y;
	} else {
		positionPacket.pickupPosition.x = 0;
	}

	vector<PlayerObject*> *playersList = gameSimulator.getPlayerObjects();

	for (size_t i = 1; i < playersList->size(); ++i) {
		Client *client = getClientFromPlayerId(((NetworkPlayerObject*)(*playersList)[i])->getId());
		if (client) {
			for (int j = 0; j < competitors->size(); ++j) {
				positionPacket.playersPosition[j].flags &= 0xF7;	//Clear myShip Flag
			}
			positionPacket.playersPosition[i].flags |= SERVER_PLAYER_MY_SHIP;
			positionPacket.miscPosition.pickup = (uint8)(*playersList)[i]->getPickupMove();
			positionPacket.miscPosition.vMouseX = ((NetworkPlayerObject*)(*playersList)[i])->getVirtualCursorPosition().x;
			positionPacket.miscPosition.vMouseY = ((NetworkPlayerObject*)(*playersList)[i])->getVirtualCursorPosition().y;
			positionPacket.miscPosition.flags = (*playersList)[i]->getHealth();
			if ((*playersList)[i]->getSpecialMoveCooldown()) {
				positionPacket.miscPosition.flags |= SERVER_SPECIAL_COOLDOWN;
			}
			if ((*playersList)[i]->getSecondaryMoveCooldown()) {
				positionPacket.miscPosition.flags |= SERVER_SECONDARY_COOLDOWN;
			}
			if (ball->getIsInvisible()) {
				Competitor *shooter = ball->getShooter();
				if (shooter == (Competitor*)(*playersList)[i]) {
					positionPacket.ballPosition.flags &= ~SERVER_BALL_IS_INVISIBLE;
				} else {
					positionPacket.ballPosition.flags |= SERVER_BALL_IS_INVISIBLE;
				}
			}
			engine->sendDataPacket((uint8*)&positionPacket, sizeof(DataPacketPositions), client->clientAddr);
		}
	}
}

HostNetworkGameManager::Client *HostNetworkGameManager::getClientFromPlayerId(unsigned int id) {
	Client *retVal = 0;
	for (Client &client : clients) {
		if (client.clientAddr.sin_addr.s_addr == id) {
			retVal = &client;
			break;
		}
	}
	return retVal;
}

void HostNetworkGameManager::cleanUpGame() {
	backgroundStreamThreadFunctionSwitch = false;
	while (backgroundStreamThreadFunctionIsRunning) {
		crossPlatformSleep(1);
	}
	NetworkGameManager::cleanUpGame();
	gameSimulator.cleanUpGame();
	clients.clear();
	engine->closeNetwork();
	engine->setRenderHUD(false);
	engine->showCursor(true);
	hostPlayerWasVictorious = false;
	if (backgroundStreamBuffer) {
		delete [] backgroundStreamBuffer;
		backgroundStreamBuffer = 0;
	}
	isPaused = false;
}