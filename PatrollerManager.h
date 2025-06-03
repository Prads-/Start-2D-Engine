#ifndef PATROLLER_MANAGER_H
#define PATROLLER_MANAGER_H

#include "EnemyManager.h"

class PatrollerManager : public EnemyManager {
private:
	Image projectileSprite, spriteAttack, spriteCannon, spriteDeath;

public:
	PatrollerManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif