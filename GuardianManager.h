#ifndef GUARDIAN_MANAGER_H
#define GUARDIAN_MANAGER_H

#include "EnemyManager.h"
#include <list>
#include "Guardian.h"
#include "Image.h"
#include "Grid.h"

class GuardianManager : public EnemyManager {
private:
	Image spriteBaseBoostEffect, spriteChargeState, spriteDeath, spriteDefaultState, spritePreparationState, spriteShieldCollisionEffect;
	Grid **gridMap;
	bool allocateGridMapSpace;

public:
	GuardianManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);
	~GuardianManager();

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
	void updateGridMap();
};

#endif