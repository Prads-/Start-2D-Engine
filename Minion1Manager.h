#ifndef MINION_1_MANAGER_H
#define MINION_1_MANAGER_H

#include "EnemyManager.h"
#include "Image.h"
#include <list>
#include "Minion1.h"

class Minion1Manager : public EnemyManager {
private:
	float speed;
	Image spriteAttack, spriteDeath;

public:
	Minion1Manager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif