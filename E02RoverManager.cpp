#include "E02RoverManager.h"
#include "EnemyScore.h"
#include "E02Rover.h"

E02RoverManager::E02RoverManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) 
				: EnemyManager(engine, bBoxes, immobilisers) {
	spritePreparationDetail.loadImage("Images/enemies/E02Rover/PreparationDetail.png");
	spriteDeath.loadImage("Images/enemies/E02Rover/Death.png");
	spriteAttackDetail.loadImage("Images/enemies/E02Rover/AttackDetail.png");
	spriteShield.loadImage("Images/enemies/E02Rover/ShieldFull.png");
	shieldRecovery.openAnimationFile("Images/enemies/E02Rover/ShieldRecovery.anim");
	loadSpriteListFile("Images/enemies/E02Rover/sprites.dat");
	createShadowSprite();
	killScore = ENEMY_SCORE_E02ROVER;
}

void E02RoverManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	E02Rover::SpritePack spritePack(&spritePreparationDetail, &spriteAttackDetail, &spriteDeath, &spriteShield);
	E02Rover *e02Rover = new E02Rover(engine, bBoxes, 2, ball, &shadowSprite, shadowPeriod, immobilisers, &sprites, player, position, &spritePack,
		&shieldRecovery);
	if (randomPosition) {
		e02Rover->setAtRandomPosition();
		e02Rover->calculateSensorBB();
	}
	enemies.push_back(e02Rover);
	enemiesCollection.push_back(e02Rover);
}