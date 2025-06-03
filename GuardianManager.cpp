#include "GuardianManager.h"
#include "EnemyScore.h"
using namespace std;

GuardianManager::GuardianManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) 
				: EnemyManager(engine, bBoxes, immobilisers) {
	gridMap = 0;
	allocateGridMapSpace = true;
	spriteBaseBoostEffect.loadImage("Images/enemies/guardian/BaseBoosterEffect.png");
	spriteChargeState.loadImage("Images/enemies/guardian/ChargeState.png");
	spriteDeath.loadImage("Images/enemies/guardian/Death.png");
	spriteDefaultState.loadImage("Images/enemies/guardian/DefaultState.png");
	spritePreparationState.loadImage("Images/enemies/guardian/PreparationState.png");
	spriteShieldCollisionEffect.loadImage("Images/enemies/guardian/ShieldCollisionEffect.png");
	loadSpriteListFile("Images/enemies/guardian/sprites.dat");
	createShadowSprite(0xCF);
	killScore = ENEMY_SCORE_GUARDIAN;
}

GuardianManager::~GuardianManager() {
	if (gridMap) {
		engine->destroyGridMap(gridMap, GUARDIAN_WIDTH);
	}
}

void GuardianManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	Guardian::SpritePack spritePack(&spriteBaseBoostEffect, &spriteChargeState, &spriteDeath, &spriteDefaultState, &spritePreparationState, &spriteShieldCollisionEffect);
	Guardian *guardian = new Guardian(engine, &shadowSprite, shadowPeriod, bBoxes, 2, ball, player, gridMap, position, immobilisers, &sprites, &spritePack);
	if (randomPosition) {
		guardian->setAtRandomPosition();
	}
	enemies.push_back(guardian);
	enemiesCollection.push_back(guardian);
}

void GuardianManager::updateGridMap() {
	engine->createGridMap(&gridMap, bBoxes, GUARDIAN_WIDTH, GUARDIAN_HEIGHT, allocateGridMapSpace);
	allocateGridMapSpace = false;
}