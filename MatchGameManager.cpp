#include "MatchGameManager.h"
#include "RandomLevelGenerator.h"
using namespace std;
using namespace FileLoader;

MatchGameManager::MatchGameManager(StartEngine *engine, function<void()> backToMainCallBack, Image *font, 
								   Image *pickupSprites, Image *pickupNoRespawn, Image *pickupCross, Image *shootFrame) 
				: GameManager(engine, backToMainCallBack, font, pickupSprites, pickupNoRespawn), countDownTimer(120) {
	this->shootFrame = shootFrame;
	this->pickupCross = pickupCross;
}

MatchGameManager::~MatchGameManager() {
	cleanUpGame();
}

void MatchGameManager::cleanUpGame() {
	GameManager::cleanUpGame();
	for (int i = 0; i < aiObjects.size(); ++i) {
		delete aiObjects[i];
	}
	aiObjects.clear();
	engine->showCursor(true);
	engine->setRenderHUD(false);
}

MatchGameManager::GameResult MatchGameManager::getResult() {
	return result;
}

void MatchGameManager::startGame(GameInformation gameInformation) {
	cleanUpGame();
	result = GAME_UNDECIDED;
	engine->setGridOffsets(0, 0);
	GameManager::startGame(gameInformation);
	doMiscelleneousInitialisation(gameInformation);
	engine->showCursor(false);
	engine->setRenderHUD(true);
}

void MatchGameManager::startRandomLevel(GameInformation gameInformation) {
	cleanUpGame();
	result = GAME_UNDECIDED;
	engine->setGridOffsets(0, 0);
	pushBackEdgeBB();
	RandomLevelGenerator randomLevelGenerator(engine, &bBoxes);
	ball = new BallObject(engine, &bBoxes);
	ball->setAtRandomPosition();
	doMiscelleneousInitialisation(gameInformation);
	engine->showCursor(false);
	engine->setRenderHUD(true);
}

void MatchGameManager::doMiscelleneousInitialisation(GameInformation gameInformation) {
	createOpponents(gameInformation);
	player->setAIDifficulty(gameInformation.difficulty);
	for (int i = 0; i < aiObjects.size(); ++i) {
		aiObjects[i]->setAIDifficulty(gameInformation.difficulty);
	}
	pickupItem = PickupItem::getInstance(engine, &bBoxes);
	countDown = true;
	countDownTimer.startTimer();
}

void MatchGameManager::renderPauseScreen() {
	engine->clearScreen();
	drawObjects();
	engine->makeFrameMonochrome();
	btnBackToMain.draw();
	btnResume.draw();
	engine->presentScreen();
	
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnBackToMain.update(wasMouseLeftToggled);
	btnResume.update(wasMouseLeftToggled);
}

void MatchGameManager::renderCountdown() {
	engine->clearScreen();
	drawObjects();
	engine->draw(&ready, 0, false, false, true, 212, 200);
	engine->presentScreen();

	if (countDownTimer.checkTimer()) {
		countDown = false;
	}
}

void MatchGameManager::createOpponents(GameInformation gameInformation) {
	for (int i = 0; i < 3; ++i) {
		if (gameInformation.ai[i]) {
			aiObjects.push_back(new AIObject(engine, ball, &bBoxes, &immobilisers, gameInformation.ai[i]->getAIVariables(), gameInformation.ai[i]->getPath()));
			aiObjects[aiObjects.size() - 1]->setStartLook(Vector2f(0.0f, -1.0f));
			aiObjects[aiObjects.size() - 1]->setAtRandomPosition();
		}
	}
	player = new PlayerObject(engine, ball, font, &bBoxes, &immobilisers, gameInformation.player->getPath(), 
		gameInformation.player->getPlayerVariables(), pickupSprites, pickupCross, COMPETITOR_START_HEALTH);
	player->setStartLook(Vector2f(0.0f, -1.0f));
	player->setAtRandomPosition();

	for (int i = 0; i < aiObjects.size(); ++i) {
		vector<const Competitor*> opponents;
		opponents.push_back(player);
		for (int j = 0; j < aiObjects.size(); ++j) {
			if (i != j) {
				opponents.push_back(aiObjects[j]);
			}
		}
		aiObjects[i]->setOpponents(&opponents);
	}

	vector<const Enemy*> competitors;
	competitors.push_back(player);
	for (int i = 0; i < aiObjects.size(); ++i) {
		competitors.push_back(aiObjects[i]);
	}
	ball->setEnemies(competitors);
}

void MatchGameManager::renderGame() {
	engine->clearScreen();

	updateObjects();
	drawObjects();
	
	engine->presentScreen();

	if (player->isDead()) {
		result = GAME_AI_WON;
	} else {
		bool allAIDead = true;
		for (int i = 0; i < aiObjects.size(); ++i) {
			if (!aiObjects[i]->isDead()) {
				allAIDead = false;
				break;
			}
		}
		if (allAIDead) {
			result = GAME_PLAYER_WON;
		}
	}

	if (engine->isAsciiKeyPressed(KEY_ESCAPE)) {
		pauseGame();
	}
}

void MatchGameManager::updateObjects() {
	for (int i = 0; i < aiObjects.size(); ++i) {
		if (!aiObjects[i]->isDead()) {
			aiObjects[i]->update();
		}
	}
	player->update();
	ball->update();
	updateImmobilisers();
	if (!pickupItem->getIsActive()) {
		pickupItem->update();
	}
	updateExplosion();
}

void MatchGameManager::drawObjects() {
	for (int i = 0; i < aiObjects.size(); ++i) {
		if (!aiObjects[i]->isDead()) {
			aiObjects[i]->draw();
		}
	}
	if (player->hasBall()) {
		engine->draw(shootFrame, 0, false, false, true, 287, 0);
	}
	player->draw();
	player->presentHUD();
	ball->draw();
	drawImmobilisers();
	if (pickupItem->getIsActive()) {
		pickupItem->draw();
	}
	drawExplosion();
	renderPrioritisedSprites();
	player->drawCursor();
}

void MatchGameManager::render() {
	if (gamePaused) {
		renderPauseScreen();
	} else if (countDown) {
		renderCountdown();
	} else {
		renderGame();
	}
}

void MatchGameManager::pauseGame() {
	engine->showCursor(true);
	player->pause();
	for (int i = 0; i < aiObjects.size(); ++i) {
		aiObjects[i]->pause();
	}
	pickupItem->pause();
	explosion->pause();
	gamePaused = true;
}

void MatchGameManager::resumeGame() {
	engine->showCursor(false);
	player->resume();
	for (int i = 0; i < aiObjects.size(); ++i) {
		aiObjects[i]->resume();
	}
	pickupItem->resume();
	explosion->resume();
	gamePaused = false;
}

void MatchGameManager::renderPrioritisedSprite(const PrioritisedAsset *sprite) {
	bool alphaBlend = sprite->asset.alphaBlend;
	if (alphaBlend) {
		goto drawSprite;
	}
	if (pickupItem->getIsActive() && engine->checkCollision(&sprite->bBox, pickupItem->getBoundingBox())) {
		alphaBlend = true;
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
	for (int i = 0; i < aiObjects.size(); ++i) {
		if (!aiObjects[i]->isDead() && engine->checkCollision(&sprite->bBox, aiObjects[i]->getBoundingBox())) {
			alphaBlend = true;
			goto drawSprite;
		}
	}

drawSprite:
	engine->draw(sprite->sprite, sprite->asset.rotationAngle, sprite->asset.flipX, sprite->asset.flipY, alphaBlend, sprite->asset.x, sprite->asset.y);
}