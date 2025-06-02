#ifndef THE_EYE_MANAGER_H
#define THE_EYE_MANAGER_H

#include "EnemyManager.h"
#include "Image.h"

class TheEyeManager : public EnemyManager {
private:
	Image projectileSprite, deathSprite;

public:
	TheEyeManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif