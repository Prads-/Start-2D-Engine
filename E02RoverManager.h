#ifndef E02ROVER_MANAGER_H
#define E02ROVER_MANAGER_H

#include "EnemyManager.h"

class E02RoverManager : public EnemyManager {
private:
	Image spritePreparationDetail, spriteAttackDetail, spriteDeath, spriteShield;
	Animation shieldRecovery;

public:
	E02RoverManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif
