#include "MB01GolemManager.h"
#include "MB01Golem.h"
#include "EnemyScore.h"

MB01GolemManager::MB01GolemManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers)
				: EnemyManager(engine, bBoxes, immobilisers) {
	artilleryExplosionAnimation.openAnimationFile("Images/enemies/MB01Golem/artilleryExplosion.anim");
	mortarExplosionAnimation.openAnimationFile("Images/enemies/MB01Golem/mortarExplosion.anim");
	shieldRecoveryAnimation.openAnimationFile("Images/enemies/MB01Golem/shieldRecovery.anim");
	mortarFallAnimation.openAnimationFile("Images/enemies/MB01Golem/mortarFall.anim");
	artilleryShellSprite.loadImage("Images/enemies/MB01Golem/ArtilleryShell.png");
	turretSprite.loadImage("Images/enemies/MB01Golem/Turret.png");
	shieldFullSprite.loadImage("Images/enemies/MB01Golem/ShieldFull.png");
	shieldDamageSprite.loadImage("Images/enemies/MB01Golem/ShieldDamage.png");
	deathSprite.loadImage("Images/enemies/MB01Golem/death.png");
	sensorDeathSprite.loadImage("Images/enemies/MB01Golem/sensorDeath.png");
	loadSpriteListFile("Images/enemies/MB01Golem/sprites.dat");
	loadSpriteListFile("Images/enemies/MB01Golem/sensor.dat", &sensorSprites);
	artilleryShellAnimation.addFrame(&artilleryShellSprite);
	artilleryShellAnimation.changeAnimationInterval(0);
	createShadowSprite(0x7F);
	allocateGridMapSpace = true;
	killScore = ENEMY_SCORE_MB01GOLEM;
}

MB01GolemManager::~MB01GolemManager() {
	for (Sprite *sprite : sensorSprites) {
		delete sprite;
	}
	sensorSprites.clear();
}

void MB01GolemManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	MB01Golem::SpritePack spritePack(&artilleryExplosionAnimation, &mortarExplosionAnimation, &mortarFallAnimation, &artilleryShellAnimation, 
									&turretSprite, &shieldFullSprite, &shieldDamageSprite, &deathSprite, &sensorDeathSprite);
	MB01Golem *mb01Golem = new MB01Golem(engine, bBoxes, MB01GOLEM_HEALTH, ball, &shadowSprite, shadowPeriod, immobilisers, &sprites, player, position,
		gridMap, &sensorSprites, &spritePack, &shieldRecoveryAnimation);
	if (randomPosition) {
		mb01Golem->setAtRandomPosition();
	}
	enemies.push_back(mb01Golem);
	enemiesCollection.push_back(mb01Golem);
}

void MB01GolemManager::updateGridMap() {
	engine->createGridMap(&gridMap, bBoxes, MB01GOLEM_WIDTH + 10, MB01GOLEM_HEIGHT + 10, allocateGridMapSpace);
	allocateGridMapSpace = false;
}