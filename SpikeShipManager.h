#ifndef SPIKE_SHIP_MANAGER_H
#define SPIKE_SHIP_MANAGER_H

#include "EnemyManager.h"

class SpikeShipManager : public EnemyManager {
private:
	Image deathSprite;

public:
	SpikeShipManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif