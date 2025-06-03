#ifndef PATROLLER_H
#define PATROLLER_H

#include "PatrolRover.h"
#include "BoundingBox.h"
#include "FrameCountTimer.h"
#include "CampaignPlayer.h"
#include "PatrollerProjectile.h"
#include <list>

class Patroller : public PatrolRover {
public:
	struct SpritePack {
		Image *spriteCanon, *spriteAttack, *spriteDeath;

		SpritePack(Image *spriteCanon, Image *spriteAttack, Image *spriteDeath) {
			this->spriteAttack = spriteAttack;
			this->spriteCanon = spriteCanon;
			this->spriteDeath = spriteDeath;
		}
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_ATTACK, MODE_CONFUSE
	}mode;
	Image *projectileSprite;
	FrameCountTimer confuseTimer;
	Vector2f previousLookAt;
	std::list<PatrollerProjectile> projectiles;
	int cannonRotateDegrees;
	SpritePack spritePack;

	void normalMode();
	void attackMode();
	void confuseMode();
	void updateProjectile();
	bool checkCurrentRotation();

public:
	Patroller(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, Image *shadowSprite, int health, BallObject *ball, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, Vector2f position, 
		CampaignPlayer *player, Image *projectile, SpritePack *spritePack);

	void update();
	void draw();

	void pause();
	void resume();
};

#endif