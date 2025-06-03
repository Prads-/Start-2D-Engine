#ifndef COMPETITOR_H
#define COMPETITOR_H

#include "declarations.h"
#include "PathFindingEnemy.h"
#include "FrameCountTimer.h"
#include "BallObject.h"
#include "PickupItem.h"
#include <list>
#include "Immobiliser.h"
#include "Animation.h"
#include "AIDifficulty.h"

#define COMPETITOR_START_HEALTH			10

class Competitor : public Enemy {
protected:
	Image sprite, spriteMoving, spriteBall;
	Animation damageAnimation;
	bool isImmune, hadCollidedWithBallInLastFrame, playDamageAnimation, wasCollisionWithObstacle;
	bool _hasBall;
	Vector2f moveVec;
	PickupMove specialMove, pickupMove, secondaryAbility;
	bool specialMoveCooldown, secondaryAbilityCooldown;
	FrameCountTimer specialMoveCooldownTimer, secondaryAbilityCooldownTimer;
	bool shieldActive, batterySpeedUp, shotImmobiliser, shotBall;
	FrameCountTimer ballHoldTimer, shieldTimer, batteryTimer;
	float maxSpeed;
	AIDifficulty difficulty;
	static Animation shieldEffectAnimationFrames;
	Animation shieldEffectAnimation;

	void updateImmunity();
	void checkCollision();
	void checkCollisionWithBall();

	void shootImmobiliser();
	void activateShield();
	void updateShield();
	void activateBatterySpeedUp();
	void updateBatterySpeedUp();
	void drawShieldEffect();

	void teleportBall();
	bool callBall();
	
	void updateCooldownPeriod();

	virtual void collisionWithBall();
	virtual void checkCollisionWithPickup();
	virtual bool checkBallHoldTimer();
	virtual void takeAwayHP(int hp = 1);
	virtual void renderHUD() { }

public:
	Competitor(StartEngine *engine, BallObject *ball, std::list<Immobiliser> *immobilisers, const std::vector<BoundingBox> *bBoxes, 
		std::string characterPath, PickupMove specialThrow, PickupMove secondaryAbility, int health);

	bool hasBall() const;
	bool getIsImmobilised() const;
	void takeAwayImmunity();
	Vector2f getMoveVec() const;
	bool getShieldActive() const;
	bool getPlayDamageAnimation() const;
	bool getShotImmobiliser() const;
	bool getShotBall() const;

	PickupMove getPickupMove();
	PickupMove getSpecialMove(bool &isInCooldownPeriod);
	PickupMove getSecondaryMove(bool &isInCooldownPeriod);

	bool getSpecialMoveCooldown();
	bool getSecondaryMoveCooldown();

	virtual void pause();
	virtual void resume();
	virtual void setAIDifficulty(AIDifficulty difficulty) = 0;
	virtual void clearPickupMove();

	static Animation *getShieldEffectAnimation();
};

#endif