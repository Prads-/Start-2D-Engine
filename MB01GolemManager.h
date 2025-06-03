#ifndef MB01GOLEM_MANAGER_H
#define MB01GOLEM_MANAGER_H

#include "EnemyManager.h"
#include "Image.h"
#include "Animation.h"
#include <vector>
#include "Sprite.h"
#include "Grid.h"

class MB01GolemManager : public EnemyManager {
private:
	Animation artilleryExplosionAnimation, mortarExplosionAnimation, shieldRecoveryAnimation, mortarFallAnimation, artilleryShellAnimation;
	Image artilleryShellSprite, turretSprite, shieldFullSprite, shieldDamageSprite, deathSprite, sensorDeathSprite;
	std::vector<Sprite*> sensorSprites;
	Grid **gridMap;
	bool allocateGridMapSpace;

public:
	MB01GolemManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);
	~MB01GolemManager();

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
	void updateGridMap();
};

#endif