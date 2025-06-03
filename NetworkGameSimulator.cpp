#include "NetworkGameSimulator.h"
#include "FileLoader.h"
#include "RandomLevelGenerator.h"
using namespace FileLoader;
using namespace std;

NetworkGameSimulator::NetworkGameSimulator(StartEngine *engine) : countDownTimer(120) {
	this->engine = engine;
	ball = 0;
}

NetworkGameSimulator::~NetworkGameSimulator() {
	cleanUpGame();
}

void NetworkGameSimulator::pushBackEdgeBBs() {
	bBoxes.push_back(BoundingBox(Vector2f(0.0f, 0.0f), Vector2f(1024.0f, 20.0f)));
	bBoxes.push_back(BoundingBox(Vector2f(0.0f, 748.0f), Vector2f(1024.0f, 768.0f)));
	bBoxes.push_back(BoundingBox(Vector2f(0.0f, 0.0f), Vector2f(20.0f, 768.0f)));
	bBoxes.push_back(BoundingBox(Vector2f(1004.0f, 0.0f), Vector2f(1024.0f, 768.0f)));
}

void NetworkGameSimulator::startNetworkGame(std::vector<Character> *playerCharacters, std::vector<Character> *aiCharacters, std::string mapPath, 
											AIDifficulty difficulty, Image *pickupSprites, Image *font, const Image *pickupCross) {
	cleanUpGame();
	pushBackEdgeBBs();

	if (mapPath == "") {
		RandomLevelGenerator randomLevelGenerator(engine, &bBoxes);
	} else {
		loadLevelBoundingBox("maps/" + mapPath + "/bb.txt", &bBoxes);
	}
	
	PickupItem::getInstance(engine, &bBoxes);
	ball = new BallObject(engine, &bBoxes);
	ball->setAtRandomPosition();

	PlayerObject *player = new PlayerObject(engine, ball, font, &bBoxes, &immobilisers, (*playerCharacters)[0].getPath(), 
		(*playerCharacters)[0].getPlayerVariables(), pickupSprites, pickupCross, COMPETITOR_START_HEALTH);
	player->setAtRandomPosition();
	competitors.push_back(player);
	allCompetitors.push_back(player);
	players.push_back(player);
	for (int i = 1; i < playerCharacters->size(); ++i) {
		NetworkPlayerObject *player = new NetworkPlayerObject(engine, ball, font, &bBoxes, &immobilisers, (*playerCharacters)[i].getPath(), 
			(*playerCharacters)[i].getPlayerVariables(), pickupSprites, pickupCross);
		player->setAtRandomPosition();
		competitors.push_back(player);
		allCompetitors.push_back(player);
		players.push_back(player);
	}
	for (Character &character : *aiCharacters) {
		AIObject *aiObject = new AIObject(engine, ball, &bBoxes, &immobilisers, character.getAIVariables(), character.getPath());
		aiObject->setAtRandomPosition();
		competitors.push_back(aiObject);
		allCompetitors.push_back(aiObject);
		aiObjects.push_back(aiObject);
	}

	for (AIObject *aiObject : aiObjects) {
		vector<const Competitor*> opponents;
		for (const Competitor *competitor : competitors) {
			if (competitor != aiObject) {
				opponents.push_back(competitor);
			}
		}
		aiObject->setOpponents(&opponents);
	}

	vector<const Enemy*> enemies;
	for (const Competitor *competitor : competitors) {
		enemies.push_back(competitor);
	}
	ball->setEnemies(enemies);
}

void NetworkGameSimulator::cleanUpGame() {
	for (PlayerObject *player : players) {
		delete player;
	}
	players.clear();
	for (AIObject *aiObject : aiObjects) {
		delete aiObject;
	}
	aiObjects.clear();
	if (ball) {
		delete ball;
		ball = 0;
	}
	bBoxes.clear();
	immobilisers.clear();
	competitors.clear();
	allCompetitors.clear();
}

void NetworkGameSimulator::simulateAFrame(bool paused) {
	if (isCountingDown) {
		isCountingDown = !countDownTimer.checkTimer();
		return;
	}

	ball->update();
	//Host player
	if (!paused && !players[0]->isDead()) {
		players[0]->update();
	}
	//Client players
	for (int i = 1; i < players.size(); ++i) {
		if (!players[i]->isDead()) {
			players[i]->update();
		}
	}
	
	for (AIObject *aiObject : aiObjects) {
		if (!aiObject->isDead()) {
			aiObject->update();
		}
	}
	for (list<Immobiliser>::iterator it = immobilisers.begin(); it != immobilisers.end();) {
		if (it->getIsActive()) {
			it->update();
			++it;
		} else {
			it = immobilisers.erase(it);
		}
	}
	eliminateDeadCompetitors();
}

std::vector<PlayerObject*> *NetworkGameSimulator::getPlayerObjects() {
	return &players;
}

std::vector<AIObject*> *NetworkGameSimulator::getAIObject() {
	return &aiObjects;
}

std::list<Competitor*> *NetworkGameSimulator::getCompetitors() {
	return &competitors;
}

BallObject *NetworkGameSimulator::getBall() {
	return ball;
}

std::list<Immobiliser> *NetworkGameSimulator::getImmobilisers() {
	return &immobilisers;
}

void NetworkGameSimulator::eliminateDeadCompetitors() {
	for (list<Competitor*>::iterator it = competitors.begin(); it != competitors.end(); ) {
		if ((*it)->isDead()) {
			it = competitors.erase(it);
		} else {
			++it;
		}
	}
}

PlayerObject *NetworkGameSimulator::getHostPlayer() {
	return players[0];
}

std::vector<Competitor*> *NetworkGameSimulator::getAllCompetitors() {
	return &allCompetitors;
}

void NetworkGameSimulator::startCountdown() {
	isCountingDown = true;
	countDownTimer.startTimer();
}

bool NetworkGameSimulator::getIsCountingDown() {
	return isCountingDown;
}