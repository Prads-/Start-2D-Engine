#ifndef SENTRY_PROJECTILE_H
#define SENTRY_PROJECTILE_H

#include "EnemyProjectile.h"
#include <vector>

class SentryProjectile : public EnemyProjectile {
private:
	Image *sprite;

public:
	SentryProjectile(StartEngine *engine, Image *sprite, const std::vector<BoundingBox> *bBoxes, CampaignPlayer *player, float speed);

	void update();
	void draw();
};

#endif