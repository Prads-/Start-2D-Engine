#include "PatrollerManager.h"
#include "Patroller.h"
#include "EnemyScore.h"

PatrollerManager::PatrollerManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) 
				: EnemyManager(engine, bBoxes, immobilisers) {
    projectileSprite.loadImage("Images/enemies/Patroller/projectile.png");
    spriteAttack.loadImage("Images/enemies/Patroller/AttackEffect.png");
    spriteCannon.loadImage("Images/enemies/Patroller/BaseCannonSprite.png");
    spriteDeath.loadImage("Images/enemies/Patroller/DeathSprite.png");
    loadSpriteListFile("Images/enemies/Patroller/sprites.dat");
	createShadowSprite();
	killScore = ENEMY_SCORE_PATROLLER;
}

void PatrollerManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	Patroller::SpritePack spritePack(&spriteCannon, &spriteAttack, &spriteDeath);
	Patroller *patroller = new Patroller(engine, bBoxes, &shadowSprite, 1, ball, shadowPeriod, immobilisers, &sprites, position, player, &projectileSprite, &spritePack);
	if (randomPosition) {
		patroller->setAtRandomPosition();
		patroller->calculateSensorBB();
	}
	enemies.push_back(patroller);
	enemiesCollection.push_back(patroller);
}
