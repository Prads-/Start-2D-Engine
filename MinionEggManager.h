#ifndef MINION_EGG_MANAGER_H
#define MINION_EGG_MANAGER_H

#include "EnemyManager.h"
#include <list>
#include "Minion1Manager.h"
#include "Minion2Manager.h"
#include "Animation.h"

class MinionEggManager : public EnemyManager {
private:
	std::list<EnemyManager*> *enemyManagers;
	Minion1Manager *minion1Manager;
	Minion2Manager *minion2Manager;
	Animation holeOpenAnimation, holeCloseAnimation, eggAnimation;
	Image deathSprite;
	
public:
	MinionEggManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers, 
		std::list<EnemyManager*> *enemyManagers, Minion1Manager *minion1Manager, Minion2Manager *minion2Manager);
	
	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
	void createShadowSprite(uint8 alpha);
};

#endif