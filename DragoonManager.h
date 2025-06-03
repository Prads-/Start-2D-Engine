#ifndef DRAGOON_MANAGER_H
#define DRAGOON_MANAGER_H

#include "EnemyManager.h"
#include "Dragoon.h"
#include <list>
#include "Image.h"
#include "Grid.h"

class DragoonManager : public EnemyManager {
private:
	Image dragoonProjectileSprite, spriteAttackState, spriteDefaultState, spriteShieldEffect, spriteRecoveryState, spriteDeath;
	Animation animationShieldRecover;
	Grid **gridMap;
	bool allocateGridMapSpace;
	
public:
	DragoonManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);
	
	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
	void updateGridMap();
};

#endif