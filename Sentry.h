#ifndef SENTRY_OBJECT_H
#define SENTRY_OBJECT_H

#include "Enemy.h"
#include "FrameCountTimer.h"
#include "Image.h"
#include "SentryProjectile.h"
#include "CampaignPlayer.h"
#include <list>

class Sentry : public Enemy {
public:
	struct SpritePack {
		Image *spriteProjectile, *spriteCannon, *spriteDeath;

		SpritePack(Image *spriteProjectile, Image *spriteCannon, Image *spriteDeath) {
			this->spriteProjectile = spriteProjectile;
			this->spriteCannon = spriteCannon;
			this->spriteDeath = spriteDeath;
		}
	};

private:
	FrameCountTimer coolDownTimer;
	bool isCoolingDown;
	CampaignPlayer *player;
	SpritePack spritePack;
	std::list<SentryProjectile> projectiles;

	void updateProjectile();
	void drawProjectiles();
	void drawTargetLine();

public:
	Sentry(StartEngine *engine, Image *shadowSprite, int initialShadowPeriod, const std::vector<BoundingBox> *bBoxes, 
		Vector2f startPosition, CampaignPlayer *player, BallObject *ball, std::list<Immobiliser> *immobilisers, 
		std::vector<Sprite*> *sprites, SpritePack *spritePack);
	
	void update();
	void draw();

	void pause();
	void resume();
};

#endif