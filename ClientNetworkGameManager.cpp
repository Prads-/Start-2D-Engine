#include "ClientNetworkGameManager.h"
#include <fstream>
#include "FileLoader.h"
#include "stringExtension.h"
#include "Explosion.h"
#include "PickupItem.h"
#include "Screen.h"
#include "SfxPool.h"
#include "GameManager.h"
using namespace std;
using namespace FileLoader;
using namespace StringExtension;

ClientNetworkGameManager::ClientNetworkGameManager(StartEngine *engine, Image *font, Image *pickupSpritesImage, Image *pickupCross, 
												   Image *shootFrame, std::function<void()> cancelGameCallback,
												   std::function<void()> victoryCallback) 
						: NetworkGameManager(engine, font, pickupSpritesImage, shootFrame, pickupCross), 
						btnCancel(engine, Vector2i(0, 709), true, [this]() { cancelGame(); }),
						btnResume(engine, Vector2i(352, 250), true, [this]() { resumeGame(); }),
						btnBackToMain(engine, Vector2i(352, 332), true, [this]() { cancelGame(); }),
						ball(engine), countDownTimer(120), broadcastPacketTimer(180) {
	mode = MODE_INACTIVE;
	
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

	this->cancelGameCallback = cancelGameCallback;
	this->victoryCallback = victoryCallback;
	cursor.loadImage("Images/cursor.png");
	engine->getScreenResolution(halfScreenResolutionX, halfScreenResolutionY);
	halfScreenResolutionX /= 2;
	halfScreenResolutionY /= 2;
	isPaused = false;
}

ClientNetworkGameManager::~ClientNetworkGameManager() {
	cleanUpGame();
}

void ClientNetworkGameManager::render() {
	switch (mode) {
		case MODE_IN_GAME:
			renderGame();
			break;
		case MODE_WAITING_GAME_START:
			renderWaitingScreen();
			break;
		case MODE_WAITING_SERVER_HANDSHAKE:
			sendBroadcastPackets();
			renderWaitingScreen();
			break;
		case MODE_PRESTART:
			doPreStartStuffs();
			break;
		case MODE_CANCEL_GAME:
			cancelGame();
			break;
		case MODE_VICTORY:
			showVictoryScreen();
	}
}

void ClientNetworkGameManager::sendBroadcastPackets() {
	if (broadcastPacketTimer.checkTimer()) {
		DataPacketClientHandShake dataPacketClientHandShake = DataPacketClientHandShake();
		engine->sendDataPacket((uint8*)&dataPacketClientHandShake, sizeof(DataPacketClientHandShake));
	}
}

bool ClientNetworkGameManager::startNetworkGame(NetworkGameInformation gameInformation) {
	cleanUpGame();
	positionPacketId = 0;
	characterId = gameInformation.characterId;
	nickname = gameInformation.nickname;

	if (engine->initialiseClientNetwork("255.255.255.255", gameInformation.portNumber, gameInformation.portNumber, [this](uint8* packet) { packetRecieved(packet); })) {
		mode = MODE_WAITING_SERVER_HANDSHAKE;
		DataPacketClientHandShake dataPacketClientHandShake = DataPacketClientHandShake();
		engine->clientEnableBroadcast();
		engine->sendDataPacket((uint8*)&dataPacketClientHandShake, sizeof(DataPacketClientHandShake));
		broadcastPacketTimer.startTimer();
		return true;
	} else {
		return false;
	}
}

void ClientNetworkGameManager::renderWaitingScreen() {
	drawMutex.lock();
	engine->clearScreen(0);

	engine->draw(Screen::getBackground(), 0, false, false, false, 0, 0);
	engine->drawText("joining", Screen::getCustomFont(), 16, 5, 0xFFFF00, 448, 20);

	if (mode == MODE_WAITING_SERVER_HANDSHAKE) {
		engine->drawText("Connecting to server...", font, 32, 32, 16, 0xFF00, 40, 100);
		engine->drawText("Waiting for server response...", font, 32, 32, 16, 0xFF00, 40, 140);
	} else {
		engine->drawText("Connected to server: " + serverName, font, 32, 32, 16, 0xFF00, 40, 100);
		engine->drawText("Waiting for server to start the game...", font, 32, 32, 16, 0xFF00, 40, 140);
		if (isStreamingBackground) {
			engine->drawText("Streaming background from the server - " + 
				floatToString(((float)streamPacketId / DATA_PACKET_NUMBER_OF_STREAM_PACKET) * 100.0f, 1) + "%", font, 32, 32, 16, 0xFF00, 40, 180);
		}
	}
	btnCancel.draw();
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnCancel.update(wasMouseLeftToggled);

	engine->presentScreen();
	drawMutex.unlock();
}

void ClientNetworkGameManager::renderGame() {
	engine->clearScreen();

	for (DumbClientObject *opponent : opponents) {
		opponent->update();
		opponent->draw();
	}
	ball.draw();
	for (list<Immobiliser>::iterator it = immobilisers.begin(); it != immobilisers.end();) {
		if (it->getIsActive()) {
			it->update();
			it->draw();
			++it;
		} else {
			it = immobilisers.erase(it);
		}
	}
	Explosion *explosion = Explosion::getInstance();
	if (explosion->getIsActive()) {
		explosion->update();
		explosion->draw();
	}
	if (pickupPosition.x & SERVER_PICKUP_IS_ACTIVE) {
		engine->draw(PickupItem::getSprite(), 0, false, false, false, pickupPosition.x & SERVER_PICKUP_X, pickupPosition.y);
	}

	if (isPaused) {
		renderPauseScreen();
	} else if (isCountingDown) {
		engine->draw(GameManager::getReady(), 0, false, false, true, 212, 200);
		isCountingDown = !countDownTimer.checkTimer();
	} else {
		sendInputPacket();
	}

	engine->presentScreen();

	if (engine->isAsciiKeyPressed(KEY_ESCAPE)) {
		engine->showCursor(true);
		isPaused = true;
	}
}

void ClientNetworkGameManager::renderPauseScreen() {
	engine->makeFrameMonochrome();
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnResume.update(wasMouseLeftToggled);
	btnBackToMain.update(wasMouseLeftToggled);

	btnResume.draw();
	btnBackToMain.draw();
}

void ClientNetworkGameManager::resumeGame() {
	engine->showCursor(false);
	isPaused = false;
}

void ClientNetworkGameManager::checkPacket(DataPacketDataType *packet) {
	switch (mode) {
		case MODE_WAITING_SERVER_HANDSHAKE:
			checkPacketWhileWaitingServerHandshake(packet);
			break;
		case MODE_WAITING_GAME_START:
			checkPacketWhileWaitingServerGameStart(packet);
			break;
		case MODE_IN_GAME:
			checkPacketWhileInGame(packet);
	}
}

void ClientNetworkGameManager::checkPacketWhileWaitingServerHandshake(DataPacketDataType *packet) {
	if (packet->dpType == DPTYPE_SERVER_GAME_INFO) {
		gameInformationPacketRecieved((DataPacketServerGameInfo*)packet);
	}
}

void ClientNetworkGameManager::checkPacketWhileWaitingServerGameStart(DataPacketDataType *packet) {
	switch (packet->dpType) {
		case DPTYPE_SERVER_GAME_START:
			gameStartPacketRecieved((DataPacketServerGameStart*)packet);
			break;
		case DPTYPE_SERVER_CANCEL_GAME:
			mode = MODE_CANCEL_GAME;
			break;
		case DPTYPE_SERVER_START_BACKGROUND_STREAM:
			backgroundStreamStartPacketRecieved();
			break;
		case DPTYPE_SERVER_BACKGROUND_STREAM:
			backgroundStreamPacketRecieved((DataPacketBackgroundStream*)packet);
	}
}

void ClientNetworkGameManager::checkPacketWhileInGame(DataPacketDataType *packet) {
	switch (packet->dpType) {
		case DPTYPE_POSITIONS:
			positionPacketRecieved((DataPacketPositions*)packet);
			break;
		case DPTYPE_SERVER_CANCEL_GAME:
			mode = MODE_CANCEL_GAME;
			break;
		case DPTYPE_SERVER_VICTORY:
			mode = MODE_VICTORY;
	}
}

void ClientNetworkGameManager::gameInformationPacketRecieved(DataPacketServerGameInfo *packet) {
	gameInformation = *packet;
	serverName.clear();
	for (int i = 0; i < 20; ++i) {
		if (gameInformation.serverName[i] == 0) {
			break;
		} else {
			serverName.push_back(gameInformation.serverName[i]);
		}
	}

	DataPacketClientGameInfoAck gameInfoAck;
	gameInfoAck.characterId = characterId;
	for (int i = 0; i < nickname.size(); ++i) {
		gameInfoAck.name[i] = nickname[i];
		if (i == 18) {
			break;
		}
	}
	engine->clientDisableBroadcast();
	engine->sendDataPacket((uint8*)&gameInfoAck, sizeof(DataPacketClientGameInfoAck));
	mode = MODE_WAITING_GAME_START;
}

void ClientNetworkGameManager::gameStartPacketRecieved(DataPacketServerGameStart *packet) {
	mode = MODE_INACTIVE;
	
	if (gameInformation.mapId >= 0) {
		vector<string> mapList;
		loadMapList(&mapList);
		string mapPath = mapList[gameInformation.mapId];
	
		drawMutex.lock();
		engine->createBackground(mapPath, &prioritisedAssets,&prioritisedImageCollector);
		drawMutex.unlock();
	}

	int opponentCounter = 0;
	while(opponentCounter < gameInformation.numberOfPlayer) {
		DumbClientObject *opponent = new DumbClientObject(engine, packet->characterIds[opponentCounter], font, &cursor, shootFrame, pickupSprites, pickupCross, &immobilisers);
		++opponentCounter;
		opponents.push_back(opponent);
		allOpponents.push_back(opponent);
	}
	if (gameInformation.hasBots) {
		while (opponentCounter < MAXIMUM_NUMBER_OF_CLIENTS) {
			DumbClientObject *opponent = new DumbClientObject(engine, packet->characterIds[opponentCounter], font, &cursor, shootFrame, pickupSprites, pickupCross, &immobilisers);
			++opponentCounter;
			opponents.push_back(opponent);
			allOpponents.push_back(opponent);
		}
	}
	
	mode = MODE_PRESTART;
}

void ClientNetworkGameManager::positionPacketRecieved(DataPacketPositions *packet) {
	if (packet->dpId > positionPacketId) {
		positionPacketId = packet->dpId;
		int counter = 0;
		for (DumbClientObject *opponent : allOpponents) {
			if (opponent->isDead()) {
				++counter;
			} else {
				opponent->setPositionPacket(packet->playersPosition[counter++], packet->miscPosition);
			}
		}
		ball.setPositionPacket(packet->ballPosition);
		if (!(pickupPosition.x & SERVER_PICKUP_IS_ACTIVE) && (packet->pickupPosition.x & SERVER_PICKUP_IS_ACTIVE)) {
			engine->playAudioBuffer(SfxPool::getSfxPickupSpawn());
		}
		pickupPosition = packet->pickupPosition;
		eliminateDeadOpponents();
	}
}

void ClientNetworkGameManager::backgroundStreamStartPacketRecieved() {
	DataPacketBackgroundStreamAck streamStartAck;
	streamStartAck.packetId = -1;
	engine->sendDataPacket((uint8*)&streamStartAck, sizeof(DataPacketBackgroundStreamAck));
	streamPacketId = 0;
	isStreamingBackground = true;
}

void ClientNetworkGameManager::backgroundStreamPacketRecieved(DataPacketBackgroundStream *packet) {
	if (streamPacketId == packet->packetId) {
		uint8 *backgroundBuffer = (uint8*)engine->getBackgroundBuffer();
		int offset = streamPacketId * ((DATA_PACKET_STREAM_DATA_SIZE / 3) * 4);
		for (int i = 0; i < DATA_PACKET_STREAM_DATA_SIZE; ++offset) {
			backgroundBuffer[offset++] = packet->backgroundData[i++];
			backgroundBuffer[offset++] = packet->backgroundData[i++];
			backgroundBuffer[offset++] = packet->backgroundData[i++];
		}
		++streamPacketId;
	}
	DataPacketBackgroundStreamAck streamAck;
	streamAck.packetId = packet->packetId;
	engine->sendDataPacket((uint8*)&streamAck, sizeof(DataPacketBackgroundStreamAck));
}

void ClientNetworkGameManager::cleanUpGame() {
	engine->showCursor(true);
	mode = MODE_INACTIVE;
	engine->closeNetwork();
	NetworkGameManager::cleanUpGame();
	engine->setRenderHUD(false);
	for (DumbClientObject *opponent : allOpponents) {
		delete opponent;
	}
	allOpponents.clear();
	opponents.clear();
	immobilisers.clear();
	iAmDead = false;
	isStreamingBackground = false;
	isPaused = false;
}

void ClientNetworkGameManager::cancelGame() {
	cleanUpGame();
	cancelGameCallback();
}

void ClientNetworkGameManager::showVictoryScreen() {
	cleanUpGame();
	victoryCallback();
}

void ClientNetworkGameManager::sendInputPacket() {
	if (!iAmDead) {
		DataPacketClientInput inputPacket;
		engine->getMouseXY(inputPacket.mouseX, inputPacket.mouseY);
		engine->setMouseCursorInScreenSpace(halfScreenResolutionX, halfScreenResolutionY);

		if (engine->isKeyPressed(INPUT_KEY_UP)) {
			inputPacket.inputFlags |= CLIENT_INPUT_UP_KEY;
		}
		if (engine->isKeyPressed(INPUT_KEY_DOWN)) {
			inputPacket.inputFlags |= CLIENT_INPUT_DOWN_KEY;
		}
		if (engine->isKeyPressed(INPUT_KEY_LEFT)) {
			inputPacket.inputFlags |= CLIENT_INPUT_LEFT_KEY;
		}
		if (engine->isKeyPressed(INPUT_KEY_RIGHT)) {
			inputPacket.inputFlags |= CLIENT_INPUT_RIGHT_KEY;
		}
		if (engine->wasMouseLeftToggled()) {
			inputPacket.inputFlags |= CLIENT_INPUT_LEFT_MOUSE;
		}
		if (engine->isKeyPressed(INPUT_KEY_THROW_SPECIAL)) {
			inputPacket.inputFlags |= CLIENT_INPUT_MID_MOUSE;
		}
		if (engine->wasMouseRightToggled()) {
			inputPacket.inputFlags |= CLIENT_INPUT_RIGHT_MOUSE;
		}
		if (engine->wasKeyToggled(INPUT_KEY_CURSOR_TOGGLE)) {
			inputPacket.inputFlags |= CLIENT_INPUT_C_KEY;
		}
		engine->sendDataPacket((uint8*)&inputPacket, sizeof(DataPacketClientInput));
	}
}

void ClientNetworkGameManager::eliminateDeadOpponents() {
	for (list<DumbClientObject*>::iterator it = opponents.begin(); it != opponents.end(); ) {
		if ((*it)->isDead()) {
			if ((*it)->isMe()) {
				iAmDead = true;
			}
			it = opponents.erase(it);
			engine->playAudioBuffer(SfxPool::getSfxPlayerDeath());
		} else {
			++it;
		}
	}
}

void ClientNetworkGameManager::doPreStartStuffs() {
	engine->setRenderHUD(true);
	engine->showCursor(false);
	isCountingDown = true;
	countDownTimer.startTimer();
	mode = MODE_IN_GAME;
}