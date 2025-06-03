#ifndef DRAGOON_PROJECTILE_H
#define DRAGOON_PROJECTILE_H

#include "EnemyProjectile.h"
#include "Image.h"

class DragoonProjectile : public EnemyProjectile {
private:
	Image *sprite;
	Vector2f startPosition;

public:
	DragoonProjectile(StartEngine *engine, Image *sprite, const std::vector<BoundingBox> *bBoxes, CampaignPlayer *player, Vector2f position, Vector2f lookAt, float speed);

	void update();
	void draw();

	Vector2f getStartPosition();
};

#endif