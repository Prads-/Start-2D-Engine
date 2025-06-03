#ifndef MINION_2_MANAGER_H
#define MINION_2_MANAGER_H

#include "EnemyManager.h"
#include "Minion2.h"

class Minion2Manager : public EnemyManager {
private:
	Image spriteDefaultState, spriteAttackState, spriteChasingState, spriteConfusedState, spriteHitStunnedState, spriteConfusedEffect,
			spriteSightDetectionEffect, spriteDeath, spriteAttackEffect;
	float speed, chasingSpeed;

public:
	Minion2Manager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif