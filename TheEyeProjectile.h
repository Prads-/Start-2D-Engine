#ifndef THE_EYE_PROJECTILE_H
#define THE_EYE_PROJECTILE_H

#include "EnemyProjectile.h"
#include "Image.h"

class TheEyeProjectile : public EnemyProjectile {
private:
	bool isActive;
	const Image *sprite;

public:
	TheEyeProjectile(StartEngine *engine, CampaignPlayer *player, Vector2f direction, float speed, Vector2f position, const Image *sprite);

	void update();
	void draw();

	bool getIsActive();
};

#endif