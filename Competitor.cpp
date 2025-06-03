#include "Competitor.h"
#include "PickupItem.h"
#include "Explosion.h"
#include "AbilityCoolDownValues.h"
#include "SfxPool.h"

Animation Competitor::shieldEffectAnimationFrames("Images/Shield.anim");

Competitor::Competitor(StartEngine *engine, BallObject *ball, std::list<Immobiliser> *immobilisers, const std::vector<BoundingBox> *bBoxes,
		 std::string characterPath, PickupMove specialThrow, PickupMove secondaryAbility, int health) : 
		Enemy(engine, bBoxes, health, ball, 0, false, immobilisers, 0), ballHoldTimer(600), 
		shieldTimer(240), batteryTimer(300) {
	isImmune = false;
	hadCollidedWithBallInLastFrame = false;
	moveVec = Vector2f(0.0f, 0.0f);
	pickupMove = PICKUP_NONE;
	shieldActive = false;
	batterySpeedUp = false;
	specialMoveCooldown = false;
	secondaryAbilityCooldown = false;
	specialMove = specialThrow;
	this->secondaryAbility = secondaryAbility;
	playDamageAnimation = false;

	AbilityCoolDownValue abilityCoolDownValue;
	specialMoveCooldownTimer.changeInterval(abilityCoolDownValue.getCooldownValue(specialMove));
	secondaryAbilityCooldownTimer.changeInterval(abilityCoolDownValue.getCooldownValue(secondaryAbility));

	sprite.loadImage("characters/" + characterPath + "/sprite.png");
	spriteBall.loadImage("characters/" + characterPath + "/ball.png");
	spriteMoving.loadImage("characters/" + characterPath + "/moving.png");
	damageAnimation.openAnimationFile("characters/" + characterPath + "/damage.anim");

	shieldEffectAnimation.linkAnimationFrames(&shieldEffectAnimationFrames);
	shieldEffectAnimation.changeAnimationInterval(shieldTimer.getInterval() / shieldEffectAnimation.getNumberOfFrames());
	shieldEffectAnimation.setLoop(false);

	shotImmobiliser = false;
	shotBall = false;
}

void Competitor::updateImmunity() {
	if (isImmune) {
		if (ball->getSpeed() == 0.0f || ball->taken()) {
			isImmune = false;
		}
	}
}

void Competitor::checkCollision() {
	if (checkCollisionWithObstacles(moveVec)) {
		wasCollisionWithObstacle = true;
	} else {
		wasCollisionWithObstacle = false;
	}
	checkCollisionWithBall();
	checkCollisionWithPickup();
	checkCollisionWithImmobilisers();
}

void Competitor::checkCollisionWithPickup() {
	PickupItem *pickupItem = PickupItem::getInstance();
	if (pickupItem->getIsActive() && engine->checkCollision(&bBox, pickupItem->getBoundingBox())) {
		pickupMove = pickupItem->getPickupMove();
		pickupItem->setInactive();
		renderHUD();
	}
}

void Competitor::checkCollisionWithBall() {
    Vector2f midPoint = getMidPosition(), ballMidPoint = ball->getMidPosition();
    if (!ball->taken() && engine->testCollision(&midPoint, &ballMidPoint, radius, ball->getRadius())) {
		collisionWithBall();
		hadCollidedWithBallInLastFrame = true;
	} else {
		Explosion *explosion = Explosion::getInstance();
        Vector2f explosionMidPoint = explosion->getMidPosition();
        if (explosion->getIsActive() && explosion->getShooter() != this && engine->testCollision(&midPoint, &explosionMidPoint, radius, Explosion::getInstance()->getRadius())) {
			if (!isImmune && !hadCollidedWithBallInLastFrame) {
				if (!shieldActive) {
					takeAwayHP();
				}
				hadCollidedWithBallInLastFrame = true;
			}
		} else {
			hadCollidedWithBallInLastFrame = false;
		}
	}
}

bool Competitor::hasBall() const {
	return _hasBall;
}

void Competitor::collisionWithBall() {
	if (ball->getSpeed() > BALL_CATCHABLE_SPEED || (ball->getBallType() == BALL_TYPE_CALL_BALL && ball->getCaller() != this)) {
		if (!isImmune && !hadCollidedWithBallInLastFrame) {
			if (!shieldActive) {
				takeAwayHP();
				ball->setHitTimer();
			}
		}
	} else {
		_hasBall = true;
		ball->setTaken(true);
		ballHoldTimer.startTimer();
		renderHUD();
	}
}

PickupMove Competitor::getPickupMove() {
	return pickupMove;
}

PickupMove Competitor::getSpecialMove(bool &isInCooldownPeriod) {
	isInCooldownPeriod = specialMoveCooldown;
	return specialMove;
}

PickupMove Competitor::getSecondaryMove(bool &isInCooldownPeriod) {
	isInCooldownPeriod = secondaryAbilityCooldown;
	return secondaryAbility;
}

bool Competitor::getSpecialMoveCooldown() {
	return specialMoveCooldown;
}

bool Competitor::getSecondaryMoveCooldown() {
	return secondaryAbilityCooldown;
}

void Competitor::shootImmobiliser() {
	Immobiliser immobiliser(engine);
	immobiliser.setStartLook(Vector2f(0.0f, -1.0f));
	immobiliser.setLook(lookAt);
	immobiliser.setPosition(position);
	immobiliser.setThrower(this);
	immobilisers->push_back(immobiliser);
	shotImmobiliser = true;
}

bool Competitor::getIsImmobilised() const {
	return isStunned;
}

void Competitor::activateShield() {
	shieldActive = true;
	shieldTimer.startTimer();
	shieldEffectAnimation.resetAnimation();
}

void Competitor::updateShield() {
	if (shieldActive) {
		if (shieldTimer.checkTimer()) {
			shieldActive = false;
		}
	}
}

bool Competitor::checkBallHoldTimer() {
	if (ballHoldTimer.checkTimer()) {
		takeAwayHP();
		_hasBall = false;
		ball->setBallType(BALL_TYPE_NORMAL);
		ball->setTaken(false);
		ball->setAtRandomPosition();
		return true;
	}
	return false;
}

void Competitor::takeAwayHP(int hp) {
	if (health > 0) {
		health -= hp;
		if (health <= 0) {
			engine->playAudioBuffer(SfxPool::getSfxPlayerDeath());
			deathTimer.startTimer();
		} else {
			engine->playAudioBuffer(SfxPool::getSfxPlayerHurt());
		}
		playDamageAnimation = true;
		damageAnimation.resetAnimation();
	}
}

void Competitor::activateBatterySpeedUp() {
	batterySpeedUp = true;
	batteryTimer.startTimer();
}

void Competitor::updateBatterySpeedUp() {
	if (batterySpeedUp) {
		if (batteryTimer.checkTimer()) {
			batterySpeedUp = false;
		}
	}
}

void Competitor::drawShieldEffect() {
	if (shieldActive) {
		const Image *shieldEffectFrame;
		shieldEffectAnimation.getAnimationFrame(&shieldEffectFrame);
		engine->draw(shieldEffectFrame, 0, false, false, false, position.x - 3, position.y - 3);
	}
}

void Competitor::teleportBall() {
	if (!ball->taken()) {
		ball->setBallType(BallType::BALL_TYPE_NORMAL);
		ball->setAtRandomPositionRelativeWithinRadius(position, 100);
	}
}

bool Competitor::callBall() {
	return ball->callBall(getMidPosition(), this);
}

void Competitor::pause() {
	Enemy::pause();
	ballHoldTimer.pauseTimer();
	shieldTimer.pauseTimer();
	batteryTimer.pauseTimer();
	specialMoveCooldownTimer.pauseTimer();
	secondaryAbilityCooldownTimer.pauseTimer();
}

void Competitor::resume() {
	Enemy::resume();
	ballHoldTimer.resumeTimer();
	shieldTimer.resumeTimer();
	batteryTimer.resumeTimer();
	specialMoveCooldownTimer.resumeTimer();
	secondaryAbilityCooldownTimer.resumeTimer();
}

void Competitor::clearPickupMove() {
	pickupMove = PICKUP_NONE;
	renderHUD();
}

Animation *Competitor::getShieldEffectAnimation() {
	return &shieldEffectAnimationFrames;
}

void Competitor::updateCooldownPeriod() {
	if (specialMoveCooldown) {
		if (specialMoveCooldownTimer.checkTimer()) {
			specialMoveCooldown = false;
		}
		if (_hasBall) {
			renderHUD();
		}
	}
	if (secondaryAbilityCooldown) {
		if (secondaryAbilityCooldownTimer.checkTimer()) {
			secondaryAbilityCooldown = false;
		}
		if (!_hasBall) {
			renderHUD();
		}
	}
}

void Competitor::takeAwayImmunity() {
	isImmune = false;
}

Vector2f Competitor::getMoveVec() const {
	return moveVec;
}

bool Competitor::getShieldActive() const {
	return shieldActive;
}

bool Competitor::getPlayDamageAnimation() const {
	return playDamageAnimation;
}

bool Competitor::getShotImmobiliser() const {
	return shotImmobiliser;
}

bool Competitor::getShotBall() const {
	return shotBall;
}
