#include "CampaignGameManager.h"
#include <string>
#include "FileLoaderException.h"
#include "EnemiesId.h"
#include "MatchGameManager.h"
#include <sstream>
#include "timer.h"
#include "stringExtension.h"
#include "ConfigFile.h"
#include "Screen.h"
#include "SfxPool.h"
using namespace StringExtension;
using namespace FileLoader;
using namespace std;

CampaignGameManager::CampaignGameManager(StartEngine *engine, std::function<void()> backToMainCallBack, Image *font, 
										 const std::vector<Arena*> *arenas, MatchGameManager *matchGameManager, Image *pickupSprites, 
										 Image *pickupNoRespawn, const Image *pickupCross)
					: GameManager(engine, backToMainCallBack, font, pickupSprites, pickupNoRespawn), sweeperManager(engine, &bBoxes, &immobilisers), sentryManager(engine, &bBoxes, &immobilisers), 
					guardianManager(engine, &bBoxes, &immobilisers), dragoonManager(engine, &bBoxes, &immobilisers), minion1Manager(engine, &bBoxes, &immobilisers),
					minion2Manager(engine, &bBoxes, &immobilisers), proximityDroneManager(engine, &bBoxes, &immobilisers), patrollerManager(engine, &bBoxes, &immobilisers),
					minionEggManager(engine, &bBoxes, &immobilisers, &enemyManagers, &minion1Manager, &minion2Manager),
					e02RoverManager(engine, &bBoxes, &immobilisers), mb01GolemManager(engine, &bBoxes, &immobilisers),
					e01SensorManager(engine, &bBoxes, &immobilisers, &e02RoverManager, &enemyManagers),
                    theEyeManager(engine, &bBoxes, &immobilisers), spikeShipManager(engine, &bBoxes, &immobilisers),
					btnSaveGame(engine, Vector2i(352, 332), true, [this]() { saveGame(); }) {
	this->arenas = arenas;
	this->matchGameManager = matchGameManager;
	this->pickupCross = pickupCross;
	btnBackToMain.setPosition(Vector2i(352, 414));

	btnSaveGame.setButtonSprite(Screen::getMiddleButtonBackgroundDefault2());
	btnSaveGame.setMouseOverSprite(Screen::getMiddleButtonBackgroundHighlight());
	btnSaveGame.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	allManagersList.push_back(&sweeperManager);
	allManagersList.push_back(&sentryManager);
	allManagersList.push_back(&guardianManager);
	allManagersList.push_back(&dragoonManager);
	allManagersList.push_back(&minion1Manager);
	allManagersList.push_back(&minion2Manager);
	allManagersList.push_back(&patrollerManager);
	allManagersList.push_back(&minionEggManager);
	allManagersList.push_back(&e02RoverManager);
	allManagersList.push_back(&mb01GolemManager);
	allManagersList.push_back(&e01SensorManager);
	allManagersList.push_back(&theEyeManager);
	allManagersList.push_back(&spikeShipManager);
	allManagersList.push_back(&proximityDroneManager);

	for (EnemyManager *enemyManager : allManagersList) {
		enemyManager->setShadowPeriodInterval(300);
	}
}

CampaignGameManager::~CampaignGameManager() {
	cleanUpGame();
}

void CampaignGameManager::startNewWave() {
	if (waves.size() == 0) {
		if (isSurvivalMode) {
			continueSurvivalMode();
		} else {
			result = GAME_PLAYER_WON;
		}
	} else {
		++levelCounter;
		currentEnemyWave = waves.front();
		waves.pop_front();
		
		startWave(&currentEnemyWave);
	}
}

void CampaignGameManager::startWave(RawEnemyWave *wave) {
	ball->clearEnemies();
	makeAllEnemyManagersEmpty();
	for (EnemyManager *manager : allManagersList) {
		enemyManagers.push_back(manager);
	}
	pickupItems.clear();
	CampaignPickupItem::PickupItemSprites pickupItemSprites(pickupSprites, pickupNoRespawn);
	for (RawCampaignPickup &rawPickup : wave->pickups) {
		pickupItems.push_back(CampaignPickupItem(engine, Vector2f(rawPickup.x, rawPickup.y), rawPickup.moves, rawPickup.interval, pickupItemSprites, rawPickup.usageCount, rawPickup.respawnCount));
	}
	
	for (const RawEnemy &enemy : wave->enemies) {
		instantiateEnemyObject(enemy);
	}
	connectBallWithEnemies();
	engine->resetInputToggles();
}

void CampaignGameManager::connectBallWithEnemies() {
	vector<const Enemy*> enemiesInArena;
	enemiesInArena.push_back(player);
	for (EnemyManager *enemyManager : enemyManagers) {
		enemyManager->getEnemiesPointer(&enemiesInArena);
	}
	ball->setEnemies(enemiesInArena);
}

EnemyManager *CampaignGameManager::getEnemyManagerById(int id) {
	switch (id) {
		case ENEMY_ID_DRAGOON:
			return &dragoonManager;
		case ENEMY_ID_GUARDIAN:
			return &guardianManager;
		case ENEMY_ID_SENTRY:
			return &sentryManager;
		case ENEMY_ID_SWEEPER:
			return &sweeperManager;
		case ENEMY_ID_MINION1:
			return &minion1Manager;
		case ENEMY_ID_MINION2:
			return &minion2Manager;
		case ENEMY_ID_PROXIMITY_DRONE:
			return &proximityDroneManager;
		case ENEMY_ID_PATROLLER:
			return &patrollerManager;
		case ENEMY_ID_MINION_EGG:
			return &minionEggManager;
		case ENEMY_ID_E02ROVER:
			return &e02RoverManager;
		case ENEMY_ID_MB01GOLEM:
			return &mb01GolemManager;
		case ENEMY_ID_E01SENSOR:
			return &e01SensorManager;
		case ENEMY_ID_THE_EYE:
			return &theEyeManager;
		case ENEMY_ID_SPIKE_SHIP:
			return &spikeShipManager;
	}

	return 0;
}

void CampaignGameManager::instantiateEnemyObject(RawEnemy rawEnemyData) {
	EnemyManager *enemyManager = getEnemyManagerById(rawEnemyData.id);

	if (enemyManager) {
		enemyManager->addAnEnemy(Vector2f(rawEnemyData.x, rawEnemyData.y), true);
	}
}

void CampaignGameManager::eliminateEmptyEnemyManagers() {
	for (list<EnemyManager*>::iterator it = enemyManagers.begin(); it != enemyManagers.end();) {
		if ((*it)->isEmpty()) {
			it = enemyManagers.erase(it);
		} else {
			++it;
		}
	}
}

void CampaignGameManager::startGame(GameInformation gameInformation, int levelOffset, int chapter, bool hardMode) {
	result = GAME_UNDECIDED;
	RawLevel rawLevel;
	loadLevelData("levels/" + gameInformation.path, &rawLevel);
	engine->setGridOffsets(rawLevel.gridOffset.x, rawLevel.gridOffset.y);
	levelCounter = levelOffset;
	this->chapter = chapter;
	isHardMode = hardMode;

	const Arena *arena;
	try {
		 arena = arenas->at(rawLevel.mapId);
	} catch (out_of_range) {
		throw FileLoaderException("Arena does not exist! Level data is probably corrupted!");
	}
	gameInformation.path = *arena->getPath();
	GameManager::startGame(gameInformation);

	player = new CampaignPlayer(engine, ball, font, &bBoxes, &immobilisers, gameInformation.player->getPath(), 
		gameInformation.player->getPlayerVariables(), &pickupItems, pickupSprites, pickupCross, 1);
	player->setStartLook(Vector2f(0.0f, -1.0f));

	waves = rawLevel.waves;
	for (int i = 0; i < levelOffset; ++i) {
		waves.pop_front();
	}
	resetAllManagers();

	playerStartPostion = rawLevel.playerPosition;
	player->setPosition(playerStartPostion);
	player->resetPreviousPosition();
	ball->setPosition(playerStartPostion);

	isSurvivalMode = false;
	engine->showCursor(false);
	engine->setRenderHUD(true);
	((CampaignPlayer*)player)->setDrawHealthBar(false);
	btnSaveGame.setIsDisabled(false);
}

void CampaignGameManager::cleanUpGame() {
	GameManager::cleanUpGame();
	makeAllEnemyManagersEmpty();
	enemyManagers.clear();
	waves.clear();
	pickupItems.clear();
	engine->showCursor(true);
	engine->setRenderHUD(false);
	engine->resetInputToggles();
}

void CampaignGameManager::startRandomLevel(GameInformation gameInformation) {
	result = GAME_UNDECIDED;
	cleanUpGame();
	pushBackEdgeBB();
	RandomLevelGenerator randomLevelGenerator(engine, &bBoxes);	
	
	ball = new BallObject(engine, &bBoxes);
	player = new CampaignPlayer(engine, ball, font, &bBoxes, &immobilisers, gameInformation.player->getPath(), 
		gameInformation.player->getPlayerVariables(), &pickupItems, pickupSprites, pickupCross, 20); 
	player->setStartLook(Vector2f(0.0f, -1.0f));
	player->setAtRandomPosition();
	ball->setPosition(player->getPosition());
	
	resetAllManagers();
    vector<int> enemies = randomLevelGenerator.getEnemies();
    instantiateEnemiesById(&enemies);

	createRandomLevelPickupItem(&randomLevelGenerator);
	connectBallWithEnemies();

	isSurvivalMode = true;
	engine->showCursor(false);
	engine->setRenderHUD(true);
	btnSaveGame.setIsDisabled(true);
}

uint32 CampaignGameManager::getSurvivalModeScore() {
	return survivalModeScore;
}

void CampaignGameManager::instantiateEnemiesById(const std::vector<int> *enemiesId) {
	makeAllEnemyManagersEmpty();
	for (int enemyId : *enemiesId) {
		EnemyManager *enemyManager = getEnemyManagerById(enemyId);
		enemyManager->addAnEnemy(Vector2f(0.0f, 0.0f), true, true);
	}
	for (EnemyManager *manager : allManagersList) {
		enemyManagers.push_back(manager);
	}
}

void CampaignGameManager::createRandomLevelPickupItem(const RandomLevelGenerator *randomLevelGenerator) {
	const RawCampaignPickup *rawPickup = randomLevelGenerator->getPickupItem();
	CampaignPickupItem::PickupItemSprites pickupItemSprites(pickupSprites, pickupNoRespawn);
	pickupItems.clear();
	pickupItems.push_back(CampaignPickupItem(engine, Vector2f(0.0f, 0.0f), rawPickup->moves, rawPickup->interval, pickupItemSprites, 
		rawPickup->usageCount, rawPickup->respawnCount, &bBoxes));
	pickupItems[0].setAtRandomPosition();
}

void CampaignGameManager::continueSurvivalMode() {
	bBoxes.clear();
	pushBackEdgeBB();
	RandomLevelGenerator randomLevelGenerator(engine, &bBoxes);
	ball->setSpeed(0.0f);
	ball->setBBoxes(&bBoxes);
	ball->updateGridMap();
	player->setAtRandomPosition();
	ball->setPosition(player->getPosition());
	resetAllManagers();
    vector<int> enemies = randomLevelGenerator.getEnemies();
    instantiateEnemiesById(&enemies);
	createRandomLevelPickupItem(&randomLevelGenerator);
	connectBallWithEnemies();
}

GameManager::GameResult CampaignGameManager::getResult() {
	return result;
}

void CampaignGameManager::render() {
	if (gamePaused) {
		renderPauseScreen();
	} else {
		renderPlayScreen();
	}
}

void CampaignGameManager::renderPlayScreen() {
	eliminateEmptyEnemyManagers();
	if (enemyManagers.size() == 0) {
		startNewWave();
	} else {
		engine->clearScreen();
		updateObjects();
		drawObjects();
		engine->presentScreen();

		checkIfPlayerIsDead();
	}


	if (engine->isAsciiKeyPressed(KEY_ESCAPE)) {
		pauseGame();
	}
}

void CampaignGameManager::renderPauseScreen() {
	engine->clearScreen();
	drawObjects();
	engine->setOffsetCoordinates(0.0f, 0.0f);
	engine->makeFrameMonochrome();
	btnResume.draw();
	btnSaveGame.draw();
	btnBackToMain.draw();
	engine->presentScreen();

	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnResume.update(wasMouseLeftToggled);
	btnSaveGame.update(wasMouseLeftToggled);
	btnBackToMain.update(wasMouseLeftToggled);
}

void CampaignGameManager::checkIfPlayerIsDead() {
	if (player->isDead()) {
		if (isSurvivalMode) {
			result = GAME_SURVIVAL_MODE_ENDED;
			survivalModeScore = ball->getScore();
		} else {
			restartCurrentWave();
		}
	}
}

void CampaignGameManager::restartCurrentWave() {
	player->setPosition(playerStartPostion);
	player->resetPreviousPosition();
	player->setHealth(1);
	player->clearPickupMove();
	ball->setSpeed(0.0f);
	ball->setPosition(player->getPosition());
	enemyManagers.clear();
	startWave(&currentEnemyWave);
}

void CampaignGameManager::saveGame() {
	if (isHardMode) {
		ConfigFile::setHardSavedChapter(chapter, levelCounter - 1);
	} else {
		ConfigFile::setEasySavedChapter(chapter, levelCounter - 1);
	}
	btnSaveGame.setCaption("saved", 98, 16, 0, Screen::getCustomFont(), 0xF2D800, 0xEDBD00);
}

void CampaignGameManager::updateObjects() {
	ball->update();
	player->update();
	
	for (EnemyManager *enemyManager : enemyManagers) {
		enemyManager->updateAllEnemies();
	}

	updateImmobilisers();
	for (CampaignPickupItem &pickup : pickupItems) {
		pickup.update();
	}
	updateExplosion();
}

void CampaignGameManager::drawObjects() {
	player->draw();
	player->presentHUD();

	for (EnemyManager *enemyManager : enemyManagers) {
		enemyManager->drawAllEnemies();
	}

	drawImmobilisers();
	for (CampaignPickupItem &pickup : pickupItems) {
		pickup.draw();
	}
	drawExplosion();
	ball->draw();

	renderPrioritisedSprites();
	player->drawCursor();
	
	if (isSurvivalMode) {
		engine->drawText("Score: " + intToStr(ball->getScore()), font, 32, 32, 16, 0xFFFF00, 800, 10);
	}
}

void CampaignGameManager::renderPrioritisedSprite(const PrioritisedAsset *sprite) {
	bool alphaBlend = sprite->asset.alphaBlend;
	if (alphaBlend) {
		goto drawSprite;
	}

	if (!ball->taken() && (!ball->getIsInvisible() || ball->getShooter() == player)) {
		if (engine->checkCollision(&sprite->bBox, ball->getBoundingBox())) {
			alphaBlend = true;
			goto drawSprite;
		}
	}
	if (engine->checkCollision(&sprite->bBox, player->getBoundingBox())) {
		alphaBlend = true;
		goto drawSprite;
	}
	for (const CampaignPickupItem &pickupItem : pickupItems) {
		if (engine->checkCollision(pickupItem.getBoundingBox(), &sprite->bBox)) {
			alphaBlend = true;
			goto drawSprite;
		}
	}
	for (const EnemyManager *enemyManager : enemyManagers) {
		if (enemyManager->doesAnyEnemyCollideWithBB(&sprite->bBox)) {
			alphaBlend = true;
			goto drawSprite;
		}
	}

drawSprite:
	engine->draw(sprite->sprite, sprite->asset.rotationAngle, sprite->asset.flipX, sprite->asset.flipY, alphaBlend, sprite->asset.x, sprite->asset.y);
}

void CampaignGameManager::makeAllEnemyManagersEmpty() {
	for (EnemyManager *manager : allManagersList) {
		manager->makeEmpty();
	}
}

void CampaignGameManager::resetAllManagers() {
	for (EnemyManager *enemyManager : allManagersList) {
		enemyManager->setBall(ball);
		enemyManager->setPlayer((CampaignPlayer*)player);
		enemyManager->updateGridMap();
	}
}

void CampaignGameManager::pauseGame() {
	engine->showCursor(true);
	player->pause();
	for (EnemyManager *enemyManager : enemyManagers) {
		enemyManager->pause();
	}
	for (CampaignPickupItem &pickupItem : pickupItems) {
		pickupItem.pause();
	}
	explosion->pause();
	gamePaused = true;
	btnSaveGame.setCaption("save", 110, 16, 0, Screen::getCustomFont(), 0xF2D800, 0xEDBD00);
}

void CampaignGameManager::resumeGame() {
	engine->showCursor(false);
	player->resume();
	for (EnemyManager *enemyManager : enemyManagers) {
		enemyManager->resume();
	}
	for (CampaignPickupItem &pickupItem : pickupItems) {
		pickupItem.resume();
	}
	explosion->resume();
	gamePaused = false;
}
