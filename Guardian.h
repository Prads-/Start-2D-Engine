#ifndef GUARDIAN_H
#define GUARDIAN_H

#include "PathFindingEnemy.h"
#include "CampaignPlayer.h"
#include "Grid.h"
#include "FrameCountTimer.h"
#include <list>

#define GUARDIAN_WIDTH				40
#define GUARDIAN_HEIGHT				40

class Guardian : public PathFindingEnemy {
public:
	struct SpritePack {
		Image *spriteBaseBoostEffect, *spriteChargeState, *spriteDeath, *spriteDefaultState, *spritePreparationState, *spriteShieldCollisionEffect;
	
		SpritePack(Image *spriteBaseBoostEffect, Image *spriteChargeState, Image *spriteDeath, Image *spriteDefaultState, 
				Image *spritePreparationState, Image *spriteShieldCollisionEffect) {
			this->spriteBaseBoostEffect = spriteBaseBoostEffect;
			this->spriteChargeState = spriteChargeState;
			this->spriteDeath = spriteDeath;
			this->spriteDefaultState = spriteDefaultState;
			this->spritePreparationState = spritePreparationState;
			this->spriteShieldCollisionEffect = spriteShieldCollisionEffect;
		}
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_CHARGE_PREPARATION, MODE_CHARGE, MODE_COOLDOWN
	}mode;
	
	Image *chargePreparationSprite, *chargingSprite;
	Vector2f chargedFrom;
	FrameCountTimer coolDownTimer, shieldGlowTimer, chargePreparationTimer, hitStunnedTimer;
	Vector2f moveVec;
	CampaignPlayer *player;
	bool isHitStunned, glowShield;
	SpritePack spritePack;

	void updateCoolDown();
	void performCharge();
	void moveNormal();
	bool rotateTowards(Vector2f towards, float rotateMaxAngle);
	void faceTowardsBall();
	void chaseAfterPlayer();
	void checkCollisionWithObstacleWhenCharging();
	void coolDown();
	bool doSATCollisionTest();
	void hitStunned();

public:
	Guardian(StartEngine *engine, Image *shadowSprite, int initialShadowPeriod, const std::vector<BoundingBox> *bBoxes, int health, 
		BallObject *ball, CampaignPlayer *player, Grid **gridMap, Vector2f position, std::list<Immobiliser> *immobilisers, 
		std::vector<Sprite*> *sprites, SpritePack *spritePack);

	bool pointTowardsPosition(Vector2f position, float distanceEpsilon, Vector2f *moveVec);

	void draw();
	void update();

	void checkCollisionWithPlayer();
	void checkCollisionWithBall();

	void pause();
	void resume();
};

#endif