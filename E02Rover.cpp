#include "E02Rover.h"
#include <fstream>

E02Rover::E02Rover(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
				   int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, 
				   Vector2f position, const SpritePack *spritePack, const Animation *shieldRecovery) 
				   : PatrolRover(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites, 
				   player, position), telegraphTimer(30), attackTimer(60), recoverTimer(60), 
				   spritePack(spritePack->spritePreparationEffect, spritePack->spriteAttackEffect, spritePack->spriteDeath, spritePack->spriteShield) {
	speed = 2.5f;
	mode = MODE_NORMAL;
	radius = width / 2.0f;
	this->shieldRecovery.linkAnimationFrames(shieldRecovery);
	this->shieldRecovery.changeAnimationInterval(shieldRecovery->getAnimationInterval());
	cooldownTimer.changeInterval(60);
	directionChangeTimer.changeInterval(90);
}

void E02Rover::update() {
	if (doPreUpdate()) {
		return;
	}

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
	} else {
		previousPosition = position;
		if (isCoolingDown) {
			isCoolingDown = !cooldownTimer.checkTimer();
		}
		switch (mode) {
			case MODE_NORMAL:
				normalMode();
				break;
			case MODE_ATTACK:
				attackMode();
				break;
			case MODE_TELEGRAPH:
				telegraphMode();
				break;
			case MODE_RECOVERY:
				recoverMode();
		}
	}
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void E02Rover::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), currentRotateDegrees, false, false, false, position.x, position.y);
		switch (mode) {
			case MODE_NORMAL:
				engine->draw(spritePack.spriteShield, 0, false, false, false, position.x - 3.0f, position.y - 3.0f);
				break;
			case MODE_ATTACK:
				engine->draw(spritePack.spriteAttackEffect, currentRotateDegrees, false, false, false, position.x, position.y);
				engine->draw(spritePack.spriteShield, 0, false, false, false, position.x - 3.0f, position.y - 3.0f);
				break;
			case MODE_TELEGRAPH:
				engine->draw(spritePack.spritePreparationEffect, currentRotateDegrees, false, false, false, position.x, position.y);
				engine->draw(spritePack.spriteShield, 0, false, false, false, position.x - 3.0f, position.y - 3.0f);
				break;
			case MODE_RECOVERY:
				{
					const Image *shieldRecoveryFrame;
					shieldRecovery.getAnimationFrame(&shieldRecoveryFrame);
					engine->draw(shieldRecoveryFrame, 0, false, false, false, position.x - 3.0f, position.y - 3.0f);
				}
		}
	}

	drawStunEffect();
}

void E02Rover::checkCollisionWithBall() {
	Enemy::checkCollisionWithBall();
	if (isImmune && mode != MODE_RECOVERY) {
		mode = MODE_RECOVERY;
		recoverTimer.startTimer();
		shieldRecovery.resetAnimation();
	}
}

void E02Rover::normalMode() {
	if (checkCurrentRotation()) {
		move();
		checkChangeDirection();
		if (!isCoolingDown && player->getPosition().getDistanceSquared(position) <= 90000.0f && !checkForObstacles(player->getPosition(), width, height)) {
			mode = MODE_TELEGRAPH;
			telegraphTimer.startTimer();
		}
	}
	checkCollisionWithPlayer();
}

void E02Rover::telegraphMode() {
	if (checkForObstacles(player->getPosition(), width, height)) {
		mode = MODE_NORMAL;
		calculateFacingRotateDegrees();
		calculateLook();
		calculateSensorBB();
	} else {
		lookAt = player->getPosition() - position;
		lookAt.normalise();
		currentRotateDegrees = lookAt.getSignedAngleBetweenInDegrees(Vector2f(0.0f, -1.0f));
		if (telegraphTimer.checkTimer()) {
			mode = MODE_ATTACK;
			attackTimer.startTimer();
		}
	}
	checkCollisionWithPlayer();
}

void E02Rover::attackMode() {
	setPosition(Vector2f(lookAt.x * 6.0f + position.x, lookAt.y * 6.0f + position.y));
	if (isThereCollision()) {
		setPosition(previousPosition);
		attackToNormalMode();
	} else if (attackTimer.checkTimer()) {
		attackToNormalMode();
	}
	checkCollisionWithPlayer();
}

void E02Rover::recoverMode() {
	if (recoverTimer.checkTimer()) {
		calculateFacingRotateDegrees();
		calculateLook();
		calculateSensorBB();
		health = 2;
		mode = MODE_NORMAL;
	}
}

void E02Rover::checkCollisionWithPlayer() {
	if (engine->checkCollision(&bBox, player->getBoundingBox())) {
		player->hurtPlayer();
	}
}

void E02Rover::attackToNormalMode() {
	calculateFacingRotateDegrees();
	calculateLook();
	calculateSensorBB();
	mode = MODE_NORMAL;
	isCoolingDown = true;
	cooldownTimer.startTimer();
}

void E02Rover::pause() {
	PatrolRover::pause();
	telegraphTimer.pauseTimer();
	attackTimer.pauseTimer();
	recoverTimer.pauseTimer();
}

void E02Rover::resume() {
	PatrolRover::resume();
	telegraphTimer.resumeTimer();
	attackTimer.resumeTimer();
	recoverTimer.resumeTimer();
}