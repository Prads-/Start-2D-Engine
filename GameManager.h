#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "StartEngine.h"
#include "CampaignPlayer.h"
#include "PrioritisedAsset.h"
#include "Explosion.h"
#include <functional>
#include "Image.h"
#include <list>
#include <vector>
#include "Competitor.h"
#include "Button.h"

class GameManager {
public:
	enum GameResult {
		GAME_UNDECIDED, GAME_PLAYER_WON, GAME_AI_WON, GAME_SURVIVAL_MODE_ENDED
	};

protected:
	StartEngine *engine;
	PlayerObject *player;
	BallObject *ball;
	Image *font;
	std::vector<BoundingBox> bBoxes;
	std::vector<PrioritisedAsset> prioritisedAsset;
	std::vector<Image*> prioritisedImageCollector;
	bool gameManager;
	Explosion *explosion;
	std::list<Immobiliser> immobilisers;
	Image *pickupSprites, *pickupNoRespawn;
	static Image ready;
	std::function<void()> backToMainCallBack;
	GameResult result;
	bool gamePaused;
	
	static Image resumeButtonImage;
	Button btnResume, btnBackToMain;

	void renderPrioritisedSprites();
	virtual void renderPrioritisedSprite(const PrioritisedAsset *sprite) = 0;

	void pushBackEdgeBB();
	void loadAssetsAndBoundingBoxes(std::string arenaPath);
	void updateImmobilisers();
	void drawImmobilisers();
	void updateExplosion();
	void drawExplosion();

	void goBackToMainScreen();

public:
	struct GameInformation {
		std::string path;
		const Character *player, *ai[3];
		AIDifficulty difficulty;
	};

	GameManager(StartEngine *engine, std::function<void()> backToMainCallBack, Image *font, Image *pickupSprites, Image *pickupNoRespawn);
	virtual ~GameManager();

	virtual GameResult getResult() = 0;
	virtual void startGame(GameInformation gameInformation);
	virtual void render() = 0;
	virtual void cleanUpGame();

	virtual void pauseGame() = 0;
	virtual void resumeGame() = 0;

	static const Image *getReady();
};

#endif
