#include "SpikeShipManager.h"
#include "SpikeShip.h"
#include "EnemyScore.h"

SpikeShipManager::SpikeShipManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) 
				: EnemyManager(engine, bBoxes, immobilisers) {
	deathSprite.loadImage("Images/enemies/SpikeShip/Death.png");
	loadSpriteListFile("Images/enemies/SpikeShip/sprites.dat");
	createShadowSprite();
	killScore = ENEMY_SCORE_SPIKE_SHIP;
}

void SpikeShipManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	SpikeShip *spikeShip = new SpikeShip(engine, bBoxes, 1, ball, &shadowSprite, shadowPeriod, immobilisers, &sprites, player, &deathSprite);
	if (randomPosition) {
		spikeShip->setAtRandomPosition();
	} else {
		spikeShip->setPosition(position);
	}
	enemies.push_back(spikeShip);
	enemiesCollection.push_back(spikeShip);
}