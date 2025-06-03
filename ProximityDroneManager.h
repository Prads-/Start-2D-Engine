#ifndef PROXIMITY_DRONE_MANAGER_H
#define PROXIMITY_DRONE_MANAGER_H

#include "EnemyManager.h"

class ProximityDroneManager : public EnemyManager {
private:
	Image spriteAngleChange, spritePreparation, spriteExplosion, spriteDefaultState, spriteCooldownState,
		spriteDeath;
	Animation explosion;

public:
	ProximityDroneManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif