#ifndef CAMPAIGN_GAME_MANAGER_H
#define CAMPAIGN_GAME_MANAGER_H

#include "build.h"
#include "declarations.h"
#include <vector>
#include "CampaignPickupItem.h"
#include "EnemyManager.h"
#include "SweeperManager.h"
#include "GuardianManager.h"
#include "SentryManager.h"
#include "DragoonManager.h"
#include "Minion1Manager.h"
#include "Minion2Manager.h"
#include "ProximityDroneManager.h"
#include "PatrollerManager.h"
#include "MinionEggManager.h"
#include "E02RoverManager.h"
#include "MB01GolemManager.h"
#include "E01SensorManager.h"
#include "TheEyeManager.h"
#include "SpikeShipManager.h"
#include <list>
#include <string>
#include "Arena.h"
#include "GameManager.h"
#include "RandomLevelGenerator.h"
#include <fstream>
#include "FileLoader.h"

class MatchGameManager;

class CampaignGameManager : public GameManager {
private:
	std::vector<CampaignPickupItem> pickupItems;
	const std::vector<Arena*> *arenas;
	std::list<FileLoader::RawEnemyWave> waves;
	bool isSurvivalMode;
	uint32 survivalModeScore;
	const Image *pickupCross;
	FileLoader::RawEnemyWave currentEnemyWave;
	Vector2f playerStartPostion;
	int levelCounter, chapter;
	bool isHardMode;
	Button btnSaveGame;

	std::list<EnemyManager*> enemyManagers;
	std::vector<EnemyManager*> allManagersList;
	SweeperManager sweeperManager;
	SentryManager sentryManager;
	GuardianManager guardianManager;
	DragoonManager dragoonManager;
	Minion1Manager minion1Manager;
	Minion2Manager minion2Manager;
	ProximityDroneManager proximityDroneManager;
	PatrollerManager patrollerManager;
	MinionEggManager minionEggManager;
	E02RoverManager e02RoverManager;
	MB01GolemManager mb01GolemManager;
	E01SensorManager e01SensorManager;
	TheEyeManager theEyeManager;
	SpikeShipManager spikeShipManager;

	MatchGameManager *matchGameManager;
	
	void startNewWave();
	void startWave(FileLoader::RawEnemyWave *wave);
	void instantiateEnemyObject(FileLoader::RawEnemy rawEnemyData);
	void eliminateEmptyEnemyManagers();
	void renderPrioritisedSprite(const PrioritisedAsset *sprite);
	void makeAllEnemyManagersEmpty();
	void resetAllManagers();
	EnemyManager *getEnemyManagerById(int id);
	void connectBallWithEnemies();
	void instantiateEnemiesById(const std::vector<int> *enemiesId);
	void createRandomLevelPickupItem(const RandomLevelGenerator *randomLevelGenerator);
	void continueSurvivalMode();
	void checkIfPlayerIsDead();
	void restartCurrentWave();
	void saveGame();

	void updateObjects();
	void drawObjects();

	void renderPlayScreen();
	void renderPauseScreen();

public:
	CampaignGameManager(StartEngine *engine, std::function<void()> backToMainCallBack, Image *font, 
		const std::vector<Arena*> *arenas, MatchGameManager *matchGameManager, Image *pickupSprites, Image *pickupNoRespawn,
		const Image *pickupCross);
	~CampaignGameManager();

	GameResult getResult();
	void startGame(GameInformation gameInformation, int levelOffset, int chapter, bool hardMode);
	void render();
	void cleanUpGame();
	void startRandomLevel(GameInformation gameInformation);
	uint32 getSurvivalModeScore();

	void pauseGame();
	void resumeGame();
};

#endif