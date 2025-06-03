#include "E01SensorManager.h"
#include "E01Sensor.h"
#include "EnemyScore.h"

E01SensorManager::E01SensorManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers, 
											 E02RoverManager *e02RoverManager, std::list<EnemyManager*> *enemyManagers) 
											 : EnemyManager(engine, bBoxes, immobilisers) {
	this->e02RoverManager = e02RoverManager;
	this->enemyManagers = enemyManagers;
	spriteCooldownDetail.loadImage("Images/enemies/E01Sensor/CooldownDetail.png");
	spriteDeath.loadImage("Images/enemies/E01Sensor/Death.png");
	spawningDetail.openAnimationFile("Images/enemies/E01Sensor/SpawningDetail.anim");
	spawningDetail.setLoop(true);
	loadSpriteListFile("Images/enemies/E01Sensor/sprites.dat");
	createShadowSprite();
	killScore = ENEMY_SCORE_E01SENSOR;
}

void E01SensorManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	E01Sensor::SpritePack spritePack(&spriteCooldownDetail, &spriteDeath, &spawningDetail);
	E01Sensor *e01Sensor = new E01Sensor(engine, bBoxes, 1, ball, &shadowSprite, shadowPeriod, immobilisers, &sprites, player,
		e02RoverManager, enemyManagers, &spritePack);
	if (randomPosition) {
		e01Sensor->setAtRandomPosition();
	} else {
		e01Sensor->setPosition(position);
	}
	enemies.push_back(e01Sensor);
	enemiesCollection.push_back(e01Sensor);
}