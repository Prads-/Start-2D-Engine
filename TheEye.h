#ifndef THE_EYE_H
#define THE_EYE_H

#include "Enemy.h"
#include "CampaignPlayer.h"
#include <list>
#include "TheEyeProjectile.h"
#include "Image.h"

class TheEye : public Enemy {
private:
	enum Mode {
		MODE_ROTATION, MODE_DIRECT_SHOOT
	}mode;

	FrameCountTimer cooldownRotationShoot, cooldownDirectShoot;
	CampaignPlayer *player;
	std::list<TheEyeProjectile> projectiles;
	const Image *projectileSprite, *deathSprite;
	bool rotateShootIsCoolingDown;

	void rotationMode();
	void directShootMode();
	void updateProjectiles();
	void drawProjectiles();
	void shootAtPlayer(float projectileSpeed, Vector2f direction);
	void updateRotateShootCoolDown();

public:
	TheEye(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player,
		Image *projectileSprite, const Image *deathSprite);

	void update();
	void draw();

	void pause();
	void resume();
};

#endif