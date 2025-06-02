#include "TheEyeManager.h"
#include "TheEye.h"
#include "EnemyScore.h"

TheEyeManager::TheEyeManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers)
				: EnemyManager(engine, bBoxes, immobilisers) {
	projectileSprite.loadImage("Images/enemies/TheEye/projectile.png");
	deathSprite.loadImage("Images/enemies/TheEye/Death.png");
	loadSpriteListFile("Images/enemies/TheEye/sprites.dat");
	createShadowSprite();
	killScore = ENEMY_SCORE_THE_EYE;
}

void TheEyeManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	TheEye *theEye = new TheEye(engine, bBoxes, 1, ball, &shadowSprite, shadowPeriod, immobilisers, &sprites, player, &projectileSprite, &deathSprite);
	if (randomPosition) {
		theEye->setAtRandomPosition();
	} else {
		theEye->setPosition(position);
	}
	enemies.push_back(theEye);
	enemiesCollection.push_back(theEye);
}