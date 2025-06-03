#ifndef ENEMY_PROJECTILE
#define ENEMY_PROJECTILE

#include "InteractiveObject.h"
#include "CampaignPlayer.h"

class EnemyProjectile : public InteractiveObject {
protected:
	CampaignPlayer *player;

	void checkCollisionWithPlayer();

public:
	EnemyProjectile(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, CampaignPlayer *player);
};

#endif