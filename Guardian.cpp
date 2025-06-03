#include "Guardian.h"
#include "SAT_BoundingBox.h"
#include <fstream>
#include "Explosion.h"
#include "SfxPool.h"

#define GUARDIAN_CHARGE_DISTANCE			62500.0f
#define GUARDIAN_CHARGE_INITIATE_DISTANCE	150.0f
#define GUARDIAN_TURNING_ANGLE				1.0f

Guardian::Guardian(StartEngine *engine, Image *shadowSprite, int initialShadowPeriod, const std::vector<BoundingBox> *bBoxes, 
				   int health, BallObject *ball, CampaignPlayer *player, Grid **gridMap, Vector2f position, 
				   std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, SpritePack *spritePack) 
		: PathFindingEnemy(engine, bBoxes, health, ball, gridMap, shadowSprite, initialShadowPeriod, immobilisers, sprites), coolDownTimer(480),
			chargePreparationTimer(90), hitStunnedTimer(120), spritePack(*spritePack), shieldGlowTimer(30) {
	width = 40;
	height = 40;
	setPosition(position);
	this->gridMap = gridMap;
	this->player = player;
	this->chargePreparationSprite = chargePreparationSprite;
	this->chargingSprite = chargingSprite;
	mode = MODE_NORMAL;
	targetPosition = Vector2f(0.0f, 0.0f);
	moveVec = lookAt;
	speed = 1.5f;
	isHitStunned = false;
}

void Guardian::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), rotateDegree, false, false, false, position.x, position.y);
		if (!isHitStunned && !isStunned) {
			switch(mode) {
				case MODE_CHARGE:
					engine->draw(spritePack.spriteChargeState, rotateDegree, false, false, false, position.x - 13.0f, position.y - 13.0f);
					break;
				case MODE_CHARGE_PREPARATION:
					engine->draw(spritePack.spritePreparationState, rotateDegree, false, false, false, position.x - 7.0f, position.y - 7.0f);
					break;
				case MODE_NORMAL:
					engine->draw(spritePack.spriteDefaultState, rotateDegree, false, false, false, position.x, position.y);
				case MODE_COOLDOWN:
					engine->draw(spritePack.spriteBaseBoostEffect, rotateDegree, false, false, false, position.x - 3.0f, position.y - 3.0f);
			}
		}
		if (glowShield) {
			engine->draw(spritePack.spriteShieldCollisionEffect, rotateDegree, false, false, false, position.x, position.y);
			glowShield = !shieldGlowTimer.checkTimer();
		}
	}

	drawStunEffect();
}

void Guardian::update() {
	if (doPreUpdate()) {
		return;
	}
	
	if (isStunned) {
		updateStunnedPeriod();
	} else if (isHitStunned) {
		isHitStunned = !hitStunnedTimer.checkTimer();
	} else {
		previousPosition = position;

		switch (mode) {
			case MODE_COOLDOWN:
				updateCoolDown();
			case MODE_NORMAL:
				moveNormal();
				break;
			case MODE_CHARGE:
				performCharge();
				break;
			case MODE_CHARGE_PREPARATION:
				if (chargePreparationTimer.checkTimer()) {
					chargedFrom = position;
					mode = MODE_CHARGE;
				} else {
					moveNormal();
				}
		}
		moveVec = lookAt;

		checkCollisionWithPlayer();

		if (mode == MODE_CHARGE) {
			checkCollisionWithObstacleWhenCharging();
		} else {
			checkCollisionWithObstacles(moveVec);
		}
	}

	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void Guardian::updateCoolDown() {
	if (coolDownTimer.checkTimer()) {
		mode = MODE_NORMAL;
	}
}

void Guardian::performCharge() {
	setPosition(Vector2f(position.x + moveVec.x * 4.0f, position.y + moveVec.y * 4.0f));
	float chargedDistance = position.getDistanceSquared(chargedFrom);
	if (chargedDistance >= 40000.0f) {
		coolDown();
	}
}

void Guardian::moveNormal() {
	chaseAfterPlayer();
}

void Guardian::faceTowardsBall() {
	Vector2f guardianToBall = ball->getPosition() - position;
	guardianToBall.normalise();
	rotateTowards(guardianToBall, 2.0f * 1.5f);
}

void Guardian::chaseAfterPlayer() {
	Vector2f guardianToPlayer = player->getPosition() - position;
	guardianToPlayer.normalise();

	if (checkForObstacles(player->getMidPosition())) {
		findPathTowardsPlayer(player->getPosition(), 2500.0f, &moveVec);
	} else {
		invalidatePathTargetPosition();

		bool isFacingPlayer = rotateTowards(guardianToPlayer, 2.0f);
		if (mode != MODE_COOLDOWN && mode != MODE_CHARGE_PREPARATION) {
			float distanceToPlayer = getMidPosition().getDistanceSquared(player->getMidPosition());	
			if (isFacingPlayer && distanceToPlayer <= 40000.0f) {
				mode = MODE_CHARGE_PREPARATION;
				chargePreparationTimer.startTimer();
			} else {
				setPosition(Vector2f(position.x + moveVec.x * speed, position.y + moveVec.y * speed));
			}
		} else {
			setPosition(Vector2f(position.x + moveVec.x * speed, position.y + moveVec.y * speed));
		}
	}
}

bool Guardian::rotateTowards(Vector2f towards, float rotateMaxAngle) {
	float angle = lookAt.getSignedAngleBetween(towards);
	angle *= 180.0f / _FLOAT_PI;
	if (angle <= rotateMaxAngle && angle >= -rotateMaxAngle) {
		if (angle != 0.0f) {
			lookAt = towards;
			calculateRotateFromLook();
		}
		return true;
	} else {
		if (angle < 0) {
			rotateDegree -= rotateMaxAngle;
			if (rotateDegree < 0) {
				rotateDegree = 360 + rotateDegree;
			}
		} else {
			rotateDegree += rotateMaxAngle;
			if (rotateDegree > 360) {
				rotateDegree -= 360;
			}
		}
		calculateLookFromRotateDegree();
		return false;
	}
}

void Guardian::checkCollisionWithObstacleWhenCharging() {
	if (isThereCollision()) {
		position = previousPosition;
		coolDown();
		return;
	}
}

void Guardian::coolDown() {
	mode = MODE_COOLDOWN;
	coolDownTimer.startTimer();
}

bool Guardian::pointTowardsPosition(Vector2f position, float distanceEpsilon, Vector2f *moveVec) {
	float distance = position.getDistanceSquared(this->position);
	
	if (distance > distanceEpsilon) {
		Vector2f positionToPoint = position - this->position;
		positionToPoint.normalise();

		rotateTowards(positionToPoint, 2.0f);
		return false;
	} else {
		return true;
	}
}

void Guardian::checkCollisionWithBall() {
	bool wasCollision = false;
	if (ball->getSpeed() > BALL_CATCHABLE_SPEED && engine->checkCollision(&bBox, ball->getBoundingBox())) {
		wasCollision = doSATCollisionTest();
	} else {
		Explosion *explosion = Explosion::getInstance();
		if ((ball->getBallType() == BALL_TYPE_CALL_BALL && engine->checkCollision(&bBox, ball->getBoundingBox())) || (explosion->getIsActive() && engine->checkCollision(explosion->getBoundingBox(), &bBox))) {
			if (!isImmune) {
				takeAwayHP();
				hitStunned();
			}
			wasCollision = true;
		}
	}
	isImmune = wasCollision;
}

void Guardian::pause() {
	Enemy::pause();
	coolDownTimer.pauseTimer();
	shieldGlowTimer.pauseTimer();
	chargePreparationTimer.pauseTimer();
	hitStunnedTimer.pauseTimer();
}

void Guardian::resume() {
	Enemy::resume();
	coolDownTimer.resumeTimer();
	shieldGlowTimer.resumeTimer();
	chargePreparationTimer.resumeTimer();
	hitStunnedTimer.resumeTimer();
}

bool Guardian::doSATCollisionTest() {
	float halfWidth = width / 2.0f, halfHeight = height / 2.0f;
	SATBoundingBox satBBox(Vector2f(-halfWidth, -halfHeight), Vector2f(halfWidth, -halfHeight), Vector2f(halfWidth, halfHeight), Vector2f(-halfWidth, halfHeight));

	for (int i = 0; i < 4; ++i) {
		satBBox.p[i].rotate(engine->sin(rotateDegree), engine->cos(rotateDegree));
		satBBox.p[i].translate(halfWidth + position.x, halfHeight + position.y);
	}

	Vector2f quaterLeftDirection = satBBox.p[3] - satBBox.p[0], quaterRightDirection = satBBox.p[2] - satBBox.p[1];
	quaterLeftDirection.normalise();
	quaterRightDirection.normalise();
	float quaterScaler = height / 4.0f;

	Vector2f quarterPointLeft(satBBox.p[0].x + quaterLeftDirection.x * quaterScaler, satBBox.p[0].y + quaterLeftDirection.y * quaterScaler), 
		quarterPointRight(satBBox.p[1].x + quaterRightDirection.x * quaterScaler, satBBox.p[1].y + quaterRightDirection.y * quaterScaler);
	SATBoundingBox shieldBBox(satBBox.p[0], satBBox.p[1], quarterPointRight, quarterPointLeft), 
		backBBox(quarterPointLeft, quarterPointRight, satBBox.p[2], satBBox.p[3]);
	
	const BoundingBox *ballBBox = ball->getBoundingBox();
	SATBoundingBox satBallBBox(ballBBox->minP, Vector2f(ballBBox->maxP.x, ballBBox->minP.y), ballBBox->maxP, Vector2f(ballBBox->minP.x, ballBBox->maxP.y));

	if (engine->checkCollisionSAT(&shieldBBox, &satBallBBox)) {
		if (!isImmune) {
			ball->reverseLookAt();
			player->takeAwayImmunity();
			glowShield = true;
			shieldGlowTimer.startTimer();
			engine->playAudioBuffer(SfxPool::getSfxBallShieldBounce());
		}
		return true;
	} else if (engine->checkCollisionSAT(&backBBox, &satBallBBox)) {
		if (!isImmune) {
			takeAwayHP();
			hitStunned();
			ball->setHitTimer();
		}
		return true;
	}
	return false;
}

void Guardian::checkCollisionWithPlayer() {
	if (engine->checkCollision(&bBox, player->getBoundingBox())) {
		player->hurtPlayer();
	}
}

void Guardian::hitStunned() {
	isHitStunned = true;
	hitStunnedTimer.startTimer();
}