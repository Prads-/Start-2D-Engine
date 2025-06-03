#ifndef MATCH_GAME_MANAGER_H
#define MATCH_GAME_MANAGER_H

#include "declarations.h"
#include "AIObject.h"
#include "PlayerObject.h"
#include "BallObject.h"
#include "FileLoader.h"
#include <string>
#include <vector>
#include "PickupItem.h"
#include "Explosion.h"
#include "Immobiliser.h"
#include <list>
#include "Character.h"
#include <functional>
#include "FrameCountTimer.h"
#include "PrioritisedAsset.h"

#include "SweeperFormation.h"
#include "Sentry.h"
#include "Guardian.h"
#include "Dragoon.h"
#include "GameManager.h"

class MatchGameManager : public GameManager {
private:
	std::vector<AIObject*> aiObjects;
	PickupItem *pickupItem;
	const Image *shootFrame, *pickupCross;
	FrameCountTimer countDownTimer;
	bool countDown;
	
	void renderPauseScreen();

	void renderGame();
	void renderCountdown();
	void createOpponents(GameInformation gameInformation);
	void doMiscelleneousInitialisation(GameInformation gameInformation);
	void renderPrioritisedSprite(const PrioritisedAsset *sprite);
	void updateObjects();
	void drawObjects();

public:
	MatchGameManager(StartEngine *engine, std::function<void()> backToMainCallBack, Image *font, 
		Image *pickupSprites, Image *pickupNoRespawn, Image *pickupCross, Image *shootFrame);
	~MatchGameManager();

	void cleanUpGame();

	GameResult getResult();
	
	void startGame(GameInformation gameInformation);
	void startRandomLevel(GameInformation gameInformation);
	void render();

	void pauseGame();
	void resumeGame();
};

#endif