#include "ProximityDroneManager.h"
#include "ProximityDrone.h"
#include "EnemyScore.h"

ProximityDroneManager::ProximityDroneManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) 
						: EnemyManager(engine, bBoxes, immobilisers) {
	spriteAngleChange.loadImage("Images/enemies/proximityDrone/NewAngleStateEffect.png");
	spriteExplosion.loadImage("Images/enemies/proximityDrone/ExplosionStateEffect.png");
	spritePreparation.loadImage("Images/enemies/proximityDrone/PreparationStateEffect.png");
	spriteDefaultState.loadImage("Images/enemies/proximityDrone/DefaultStateEffect.png");
	spriteCooldownState.loadImage("Images/enemies/proximityDrone/CooldownStateEffect.png");
	spriteDeath.loadImage("Images/enemies/proximityDrone/DeathSprite.png");
	explosion.openAnimationFile("Images/enemies/proximityDrone/Explosion.anim");
	loadSpriteListFile("Images/enemies/proximityDrone/sprites.dat");
	createShadowSprite();
	killScore = ENEMY_SCORE_PROXIMITY_DRONE;
}

void ProximityDroneManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	ProximityDrone::SpritePack spritePack(&spriteCooldownState, &spriteDefaultState, &spriteExplosion, &spriteAngleChange, &spritePreparation,
		&spriteDeath);
	ProximityDrone *proximityDrone = new ProximityDrone(engine, bBoxes, 1, ball, &shadowSprite, shadowPeriod, immobilisers, &sprites, player, 
		spritePack, &explosion);
	if (randomPosition) {
		proximityDrone->setAtRandomPosition();
	} else {
		proximityDrone->setPosition(position);
	}
	enemies.push_back(proximityDrone);
	enemiesCollection.push_back(proximityDrone);
}