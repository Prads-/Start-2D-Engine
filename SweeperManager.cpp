#include "SweeperManager.h"
#include "EnemyScore.h"
using namespace std;

SweeperManager::SweeperManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers)
				: EnemyManager(engine, bBoxes, immobilisers) {
    gridMap = 0;
	allocateGridMapSpace = true;
	spriteBaseBoostEffect.loadImage("Images/enemies/sweeper/BaseBoosterEffect.png");
	spriteChargeStateEffect.loadImage("Images/enemies/sweeper/ChargeStateEffect.png");
	spriteDeath.loadImage("Images/enemies/sweeper/DeathSprite.png");
	spriteDefaultStateEffect.loadImage("Images/enemies/sweeper/DefaultStateEffect.png");
	spritePreparationStateEffect.loadImage("Images/enemies/sweeper/PreparationStateEffect.png");
	loadSpriteListFile("Images/enemies/sweeper/sprites.dat");
	createShadowSprite();
    killScore = ENEMY_SCORE_SWEEPER;
}

SweeperManager::~SweeperManager() {
	if (gridMap) {
		engine->destroyGridMap(gridMap, SWEEPER_WIDTH);
	}
}

void SweeperManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	Sweeper::SpritePack spritePack(&spriteBaseBoostEffect, &spriteChargeStateEffect, &spritePreparationStateEffect, &spriteDefaultStateEffect, &spriteDeath);
	SweeperFormation *sweeper = new SweeperFormation(engine, position, bBoxes, &shadowSprite,
		shadowPeriod, player, ball, gridMap, immobilisers, &sprites, &spritePack);
	if (randomPosition) {
		sweeper->setInRandomPosition();
	}
	enemies.push_back(sweeper);
	enemiesCollection.push_back(sweeper);
}

void SweeperManager::updateAllEnemies() {
	for (list<void*>::iterator it = enemies.begin(); it != enemies.end();) {
		((SweeperFormation*)(*it))->update();
		if (((SweeperFormation*)(*it))->isDead()) {
			it = enemies.erase(it);
			ball->addToScore(killScore);
		} else {
			++it;
		}
	}
}

void SweeperManager::drawAllEnemies() {
	for (void *sweeperFormation : enemies) {
		((SweeperFormation*)sweeperFormation)->draw();
	}
}

void SweeperManager::updateGridMap() {
	engine->createGridMap(&gridMap, bBoxes, SWEEPER_WIDTH, SWEEPER_HEIGHT, allocateGridMapSpace);
	allocateGridMapSpace = false;
}

bool SweeperManager::doesAnyEnemyCollideWithBB(const BoundingBox *bBox) const {
    for (const void *sweeper : enemies) {
        BoundingBox sweeperBBox = ((SweeperFormation*)sweeper)->getBoundingBox();
        if (engine->checkCollision(&sweeperBBox, bBox)) {
			return true;
		}
	}
	return false;
}

void SweeperManager::getEnemiesPointer(std::vector<const Enemy*> *enemiesPtrOut) {
	for (void *sweeperFormation : enemies) {
		((SweeperFormation*)sweeperFormation)->getEnemiesPointer(enemiesPtrOut);
	}
}

void SweeperManager::pause() {
	for (void *sweeperFormation : enemies) {
		((SweeperFormation*)sweeperFormation)->pause();
	}
}

void SweeperManager::resume() {
	for (void *sweeperFormation : enemies) {
		((SweeperFormation*)sweeperFormation)->resume();
	}
}
