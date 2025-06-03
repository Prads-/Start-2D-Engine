#ifndef SWEEPER_MANAGER_H
#define SWEEPER_MANAGER_H

#include "EnemyManager.h"
#include <list>
#include "SweeperFormation.h"
#include "Image.h"
#include "Grid.h"

class SweeperManager : public EnemyManager {
private:
	Image spriteBaseBoostEffect, spriteChargeStateEffect, spriteDeath, spriteDefaultStateEffect, spritePreparationStateEffect;
	Grid **gridMap;
	bool allocateGridMapSpace;

public:
	SweeperManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);
	~SweeperManager();

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
	void updateAllEnemies();
	void drawAllEnemies();
	void updateGridMap();

	bool doesAnyEnemyCollideWithBB(const BoundingBox *bBox) const;
	
	void getEnemiesPointer(std::vector<const Enemy*> *enemiesPtrOut);

	void pause();
	void resume();
};

#endif