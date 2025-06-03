#include "ProximityDrone.h"
#include "MT_RandomGenerator.h"
#include <fstream>

ProximityDrone::ProximityDrone(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
							   int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites,
							   CampaignPlayer *player, ProximityDrone::SpritePack spritePack, const Animation *explosion) 
				: Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites), angleChangeTimer(12), 
				preparationTimer(90), explosionTimer(60), coolDownTimer(30), spritePack(spritePack) {
	width = 30;
	height = 30;
	radius = 30.0f / 2.0f;
	speed = 3.0f;
	this->player = player;
	startLookAt = Vector2f(0.0f, -1.0f);
	rotateDegree = MT_RandomGenerator::randomNumber() % 360;
	calculateLookFromRotateDegree();
	mode = MODE_NORMAL;
	reboundCounter = 0;
	detectionDrawRadius = 0.0f;
	this->explosion.linkAnimationFrames(explosion);
	this->explosion.changeAnimationInterval(explosion->getAnimationInterval());
	getRandomAngle();
}

void ProximityDrone::update() {
	if (doPreUpdate()) {
		return;
	}

	if (isStunned) {
		updateStunnedPeriod();
		mode = MODE_NORMAL;
	} else { 
		switch (mode) {
			case MODE_NORMAL:
				moveNormal();
				checkIfPlayerIsWithinTheDetectionRadius();
				break;
			case MODE_ANGLE_CHANGE:
				if (angleChangeTimer.checkTimer()) {
					getRandomAngle();
					mode = MODE_NORMAL;
				}
				break;
			case MODE_PREPARATION:
				if (preparationTimer.checkTimer()) {
					mode = MODE_EXPLOSION;
					explosion.resetAnimation();
					explosionTimer.startTimer();
				}
				break;
			case MODE_EXPLOSION:
				if (explosionTimer.checkTimer()) {
					mode = MODE_COOLDOWN;
					coolDownTimer.startTimer();
				} else {
					checkCollisionWithPlayer();
				}
				break;
			case MODE_COOLDOWN:
				if (coolDownTimer.checkTimer()) {
					mode = MODE_NORMAL;
				}
				moveNormal();
		}
	}
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void ProximityDrone::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
		drawDetectionCircle(1);
		return;
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
		return;
	}
	
	const Image *baseSprite = (*sprites)[spriteCounter]->getSprite();
	uint32 detectionCircleColor = 0x5FFFFF00;
	
	engine->draw(baseSprite, 0, false, false, false, position.x, position.y);
	switch (mode) {
		case MODE_PREPARATION:
			engine->draw(spritePack.spritePreparationStateEffect, rotateDegree, false, false, false, position.x, position.y);
			detectionCircleColor = 0x5FFF0000;
			break;
		case MODE_ANGLE_CHANGE:
			engine->draw(spritePack.spriteNewAngleStateEffect, rotateDegree, false, false, false, position.x, position.y);
			break;
		case MODE_EXPLOSION:
			engine->draw(spritePack.spriteExplosionStateEffect, rotateDegree, false, false, false, position.x, position.y);
			{
				const Image *explosionFrame;
				explosion.getAnimationFrame(&explosionFrame);
				Vector2f midPosition = getMidPosition();
				engine->draw(explosionFrame, 0, false, false, true, midPosition.x - explosionFrame->getWidth() / 2, midPosition.y - explosionFrame->getHeight() / 2);
			}
			detectionCircleColor = 0x5FFF0000;
			break;
		case MODE_COOLDOWN:
			engine->draw(spritePack.spriteCooldownEffect, rotateDegree, false, false, false, position.x, position.y);
			break;
		default:
			engine->draw(spritePack.spriteDefaultStateEffect, rotateDegree, false, false, false, position.x, position.y);
	}
	if (mode != MODE_COOLDOWN) {
		drawDetectionCircle(detectionCircleColor);
	}

	drawStunEffect();
}

void ProximityDrone::pause() {
	Enemy::pause();
	angleChangeTimer.pauseTimer();
	preparationTimer.pauseTimer();
	explosionTimer.pauseTimer();
	coolDownTimer.pauseTimer();
}

void ProximityDrone::resume() {
	Enemy::resume();
	angleChangeTimer.resumeTimer();
	preparationTimer.resumeTimer();
	explosionTimer.resumeTimer();
	coolDownTimer.resumeTimer();
}

void ProximityDrone::moveNormal() {
	previousPosition = position;
	setPosition(Vector2f(position.x + lookAt.x * speed, position.y + lookAt.y * speed));
	checkCollisionWithObstacles();
}

void ProximityDrone::checkCollisionWithObstacles() {
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, &bBox)) {
			collisionWithObstacle(&obstacle);
			position = previousPosition;
			calculateBB();
			checkAngleChange();
			break;
		}
	}
}

void ProximityDrone::collisionWithObstacle(const BoundingBox *bBox) {
	bool xIsNeg = lookAt.x < 0.0f, yIsNeg = lookAt.y < 0.0f;
	CollisionSide collisionSide = getCollisionSide(bBox);
	switch (collisionSide) {
		case TOP:
			lookAt = startLookAt.getRotate(engine->sin(90 + randomAngle), engine->cos(90 + randomAngle));
			if (xIsNeg != (lookAt.x < 0.0f)) {
				lookAt.x *= -1.0f;
			}
			break;
		case BOTTOM:
			lookAt = startLookAt.getRotate(engine->sin(90 - randomAngle), engine->cos(90 - randomAngle));
			if (xIsNeg != (lookAt.x < 0.0f)) {
				lookAt.x *= -1.0f;
			}
			break;
		case RIGHT:
			lookAt = startLookAt.getRotate(engine->sin(360 - randomAngle), engine->cos(360 - randomAngle));
			if (yIsNeg != (lookAt.y < 0.0f)) {
				lookAt.y *= -1.0f;
			}
			break;
		case LEFT:
			lookAt = startLookAt.getRotate(engine->sin(randomAngle), engine->cos(randomAngle));
			if (yIsNeg != (lookAt.y < 0.0f)) {
				lookAt.y *= -1.0f;
			}
	}
	calculateRotateFromLook();
}

void ProximityDrone::checkAngleChange() {
	++reboundCounter;
	if (mode != MODE_COOLDOWN && reboundCounter >= 3) {
		mode = MODE_ANGLE_CHANGE;
		reboundCounter = 0;
		angleChangeTimer.startTimer();
	}
}

void ProximityDrone::getRandomAngle() {
	int angleDifference = 45 - 20 + 1;
	randomAngle = (MT_RandomGenerator::randomNumber() % angleDifference) + 20;
}

void ProximityDrone::checkCollisionWithPlayer() {
	if (isPlayerWithinDetectionRadius()) {
		player->hurtPlayer();
	}
}

void ProximityDrone::drawDetectionCircle(uint32 detectionCircleColor) {
	Vector2f midPosition = getMidPosition();
	engine->drawCircle(midPosition.x, midPosition.y, detectionDrawRadius, detectionCircleColor, false);
	engine->drawCircle(midPosition.x, midPosition.y, 140, detectionCircleColor, true);
	if (detectionDrawRadius >= 140.0f) {
		detectionDrawRadius = 0;
	} else {
		detectionDrawRadius += 1.4f;
	}
}

void ProximityDrone::checkIfPlayerIsWithinTheDetectionRadius() {
	if (isPlayerWithinDetectionRadius()) {
		mode = MODE_PREPARATION;
		preparationTimer.startTimer();
	}
}

bool ProximityDrone::isPlayerWithinDetectionRadius() {
	if (player->getMidPosition().getDistanceSquared(getMidPosition()) <= 19600 + PLAYER_RADIUS_SQUARED) {
		return true;
	}
	return false;
}

void ProximityDrone::drawStunEffect() {
	if (isStunned) {
		const Image *stunEffectFrame;
		stunEffectAnimation.getAnimationFrame(&stunEffectFrame);
		engine->draw(stunEffectFrame, 0, false, false, false, position.x - 8, position.y - 8);
	}
}