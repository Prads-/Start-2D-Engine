#include "DragoonManager.h"
#include "EnemyScore.h"
using namespace std;

DragoonManager::DragoonManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) 
				: EnemyManager(engine, bBoxes, immobilisers) {
	gridMap = 0;
	allocateGridMapSpace = true;
	dragoonProjectileSprite.loadImage("Images/enemies/dragoon/dragoonProjectile.png");
	spriteAttackState.loadImage("Images/enemies/dragoon/AttackStateEffect.png");
	spriteDefaultState.loadImage("Images/enemies/dragoon/DefaultStateEffect.png");
	spriteRecoveryState.loadImage("Images/enemies/dragoon/RecoveryStateEffect.png");
	spriteShieldEffect.loadImage("Images/enemies/dragoon/ShieldEffect.png");
	spriteDeath.loadImage("Images/enemies/dragoon/DeathSprite.png");
	animationShieldRecover.openAnimationFile("Images/enemies/dragoon/shield.anim");
	loadSpriteListFile("Images/enemies/dragoon/sprites.dat");
	createShadowSprite();
	killScore = ENEMY_SCORE_DRAGOON;
}

void DragoonManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	Dragoon::SpritePack spritePack(&spriteAttackState, &spriteDefaultState, &spriteRecoveryState, &spriteShieldEffect, &spriteDeath);
	Dragoon *dragoon = new Dragoon(engine, &dragoonProjectileSprite, &shadowSprite, shadowPeriod, bBoxes, ball, gridMap, player, position, 
		immobilisers, &enemies, &sprites, &spritePack, &animationShieldRecover);
	if (randomPosition) {
		dragoon->setAtRandomPosition();
	}
	enemies.push_back(dragoon);
	enemiesCollection.push_back(dragoon);
}

void DragoonManager::updateGridMap() {
	engine->createGridMap(&gridMap, bBoxes, DRAGOON_WIDTH, DRAGOON_HEIGHT, allocateGridMapSpace);
	allocateGridMapSpace = false;
}