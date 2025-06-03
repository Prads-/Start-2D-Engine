#ifndef NETWORK_GAME_SIMULATOR_H
#define NETWORK_GAME_SIMULATOR_H

#include <vector>
#include "StartEngine.h"
#include "BoundingBox.h"
#include "NetworkPlayerObject.h"
#include "AIObject.h"
#include "BallObject.h"
#include <string>
#include <list>
#include "Immobiliser.h"
#include "Character.h"

class NetworkGameSimulator {
private:
	StartEngine *engine;
	std::vector<BoundingBox> bBoxes;
	std::vector<PlayerObject*> players;
	std::vector<AIObject*> aiObjects;
	std::list<Competitor*> competitors;
	std::vector<Competitor*> allCompetitors;
	BallObject *ball;
	std::list<Immobiliser> immobilisers;
	FrameCountTimer countDownTimer;
	bool isCountingDown;

	void pushBackEdgeBBs();
	
public:
	NetworkGameSimulator(StartEngine *engine);
	~NetworkGameSimulator();

	void startNetworkGame(std::vector<Character> *playerCharacters, std::vector<Character> *aiCharacters, std::string mapPath, 
		AIDifficulty difficulty, Image *pickupSprites, Image *font, const Image *pickupCross);
	void simulateAFrame(bool paused);

	std::vector<PlayerObject*> *getPlayerObjects();
	std::vector<AIObject*> *getAIObject();
	std::list<Competitor*> *getCompetitors();
	BallObject *getBall();
	std::list<Immobiliser> *getImmobilisers();
	void eliminateDeadCompetitors();
	PlayerObject *getHostPlayer();
	std::vector<Competitor*> *getAllCompetitors();

	void cleanUpGame();

	void startCountdown();
	bool getIsCountingDown();
};

#endif