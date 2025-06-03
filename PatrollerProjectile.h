#ifndef PATROLLER_PROJECTILE_H
#define PATROLLER_PROJECTILE_H

#include "EnemyProjectile.h"
#include "Image.h"

class PatrollerProjectile : public EnemyProjectile {
private:
	Image *sprite;

public:
	PatrollerProjectile(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, CampaignPlayer *player, Vector2f position, Vector2f lookAt, float speed, Image *sprite);

	void update();
	void draw();
};

#endif