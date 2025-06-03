#include "BallObject.h"
#include "Competitor.h"
#include <cmath>
#include "Explosion.h"
#include <fstream>

BallObject::BallObject(StartEngine *engine, const std::vector<BoundingBox> *bBoxes) : InteractiveObject(engine, bBoxes), hitTimer(30) {
    sprite.loadImage("Images/Ball.png");
    spriteBallHit.loadImage("Images/BallHit.png");
    spriteBallIdle.loadImage("Images/BallIdle.png");
	spriteCallMark.loadImage("Images/callMark.png");
	spriteHitEffect.loadImage("Images/BallDamageEffect.png");

	speed = 0.0f;
	width = BALL_WIDTH;
	height = BALL_HEIGHT;
	radius = BALL_WIDTH / 2.0f;
	ballType = BALL_TYPE_NORMAL;
	isInvisible = false;
	isTaken = false;
	hasExploded = false;
	hitFlag = false;
	engine->createGridMap(&gridMap, bBoxes, width, height);
	reverseLock = false;
	score = 0;
	idleVisibilityCircle = 0.0f;
}

BallObject::~BallObject() {
	engine->destroyGridMap(gridMap, width);
}

void BallObject::setEnemies(std::vector<const Enemy*> enemies) {
	this->enemies = enemies;
}

void BallObject::decelerate() {
	float deaccelerationRate;

	if (ballType == BALL_TYPE_PINBALL) {
		deaccelerationRate = 0.05f;
	} else {
		deaccelerationRate = 0.38f;
	}

	if (speed > deaccelerationRate) {
		speed -= deaccelerationRate;
	} else {
		if (speed != 0.0f) {
			speed = 0.0f;
			doStopCheck();
			ballType = BALL_TYPE_NORMAL;
		}
	}
}

void BallObject::checkCollision() {
	size_t bbSize;

	if (ballType == BALL_TYPE_PIERCING) {
		bbSize = 4;
	} else {
		bbSize = bBoxes->size();
	}

	for (size_t i = 0; i < bbSize; ++i) {
		if (engine->checkCollision(&this->bBox, &(*bBoxes)[i])) {
			collisionWithObject(&(*bBoxes)[i]);
			position = previousPosition;
			calculateBB();
			break;
		}
	}
}

void BallObject::collisionWithObject(const BoundingBox *bBox) {
	CollisionSide collisionSide = getCollisionSide(bBox);
	switch (collisionSide) {
		case TOP:
		case BOTTOM:
			lookAt.y *= -1.0f;
			break;
		case RIGHT:
		case LEFT:
			lookAt.x *= -1.0f;
	}
	if (ballType == BALL_TYPE_HOMING) {
		ballType = BALL_TYPE_NORMAL;
	}
}

void BallObject::update() {
	if (speed != 0.0f) {
		reverseLock = false;
		previousPosition = position;

		switch (ballType) {
			case BALL_TYPE_GRANADE:
				moveGranade();
				isInvisible = false;
				break;
			case BALL_TYPE_HOMING:
				moveHoming();
				isInvisible = false;
				break;
			case BALL_TYPE_INVISIBLE:
				isInvisible = true;
				moveNormal();
				break;
			default:
				moveNormal();
				isInvisible = false;
		}

		decelerate();
		calculateBB();
		checkCollision();
	} else {
		if (ballType == BALL_TYPE_CALL_BALL) {
			moveTowardsCallPoint();
		}
	}
}
	
void BallObject::setAtShootSpeed() {
	speed = 22.0f;
}

void BallObject::setLook(Vector2f lookAt) {
	this->lookAt = lookAt;
}

void BallObject::draw() {
	if (!isTaken) {
		if (speed <= BALL_CATCHABLE_SPEED && ballType != BALL_TYPE_CALL_BALL) {
			engine->draw(&spriteBallIdle, 0, false, false, false, position.x, position.y);
			drawIdleVisibilityCircle();
		} else if (ballType != BALL_TYPE_INVISIBLE || this->shooter == enemies[0]) {
			if (hitFlag && !hitTimer.checkTimer()) {
				engine->draw(&spriteBallHit, 0, false, false, false, position.x, position.y);
				engine->draw(&spriteHitEffect, 0, false, false, false, position.x - 20, position.y - 20);
			} else {
				engine->draw(&sprite, 0, false, false, false, position.x, position.y);
				hitFlag = false;
			}
		}
		if (ballType == BALL_TYPE_CALL_BALL) {
			engine->draw(&spriteCallMark, 0, false, false, false, callPoint.x - spriteCallMark.getWidth() / 2, callPoint.y - spriteCallMark.getHeight() / 2);
		}
	}
}

void BallObject::drawIdleVisibilityCircle() {
	Vector2f midPoint = getMidPosition();
	engine->drawCircle(midPoint.x, midPoint.y, idleVisibilityCircle, 0xFF00, false);
	if (idleVisibilityCircle < 100.0f) {
		idleVisibilityCircle += 1.4f;
	} else {
		idleVisibilityCircle = 0.0f;
	}
}

bool BallObject::taken() {
	return isTaken;
}

void BallObject::setTaken(bool isTaken) {
	this->isTaken = isTaken;
	if (isTaken) {
		ballType = BALL_TYPE_NORMAL;
	}
}

void BallObject::setAtRandomPosition() {
	if (!isTaken) {
		speed = 0.0f;
		InteractiveObject::setAtRandomPosition();
	}
}

void BallObject::moveNormal() {
	position.x += lookAt.x * speed;
	position.y += lookAt.y * speed;
}

void BallObject::moveGranade() {
	moveNormal();

	float competitorDistance;
	getClosestEnemy(&competitorDistance);

	if (competitorDistance <= 2500.0f || speed <= 6.5f) {
		explode();
	}
}

void BallObject::moveHoming() {
	const Enemy *enemy = getClosestEnemy();

	if (enemy) {
		Vector2f ballToEnemy = enemy->getPosition() - position;
		ballToEnemy.normalise();
		float angle = lookAt.getSignedAngleBetween(ballToEnemy);
		if (angle > 0.1745f) {
			angle = 0.1745f;
		} else if (angle < -0.1745f) {
			angle = -0.1745f;
		}
		lookAt.x = lookAt.x * cos(angle) - lookAt.y * sin(angle);
		lookAt.y = lookAt.x * sin(angle) + lookAt.y * cos(angle);
		lookAt.normalise();
	}
	moveNormal();
}

void BallObject::explode() {
	Explosion *explosion = Explosion::getInstance();
	explosion->setShooter(shooter);
	explosion->setPosition(position - 100);
	explosion->setIsActive(true);
	setAtRandomPosition();
	hasExploded = true;
}

const Enemy *BallObject::getClosestEnemy(float *distanceOut) {
	const Enemy *closestEnemy = 0;
	float closestDistance = 1638401.0f;

	for (const Enemy *enemy : enemies) {
		if (enemy != shooter && !enemy->isDead()) {
			float distance = enemy->getPosition().getDistanceSquared(position);
			if (distance < closestDistance) {
				closestEnemy = enemy;
				closestDistance = distance;
			}
		}
	}
	if (distanceOut) {
		*distanceOut = closestDistance;
	}
	return closestEnemy;
}

void BallObject::setShooter(Competitor *shooter) {
	this->shooter = shooter;
}

Competitor *BallObject::getShooter() {
	return shooter;
}

BallType BallObject::getBallType() {
	return ballType;
}

void BallObject::setBallType(BallType type) {
	ballType = type;
}

bool BallObject::getIsInvisible() {
	return isInvisible;
}

void BallObject::setIsInvisible(bool isInvisible) {
	this->isInvisible = isInvisible;
}

bool BallObject::getHitFlag() {
	return hitFlag;
}

void BallObject::setHitTimer() {
	hitFlag = true;
	hitTimer.startTimer();
}

bool BallObject::getHasExploded() {
	bool retVal = hasExploded;
	hasExploded = false;
	return retVal;
}

void BallObject::clearEnemies() {
	enemies.clear();
}

void BallObject::doStopCheck() {
	if (ballType == BALL_TYPE_PIERCING) {
		for (const BoundingBox &obstacle : *bBoxes) {
			if (engine->checkCollision(&obstacle, &bBox)) {
				setAtRandomPosition();
				break;
			}
		}
	}
}

bool BallObject::callBall(Vector2f callPoint, Competitor *caller) {
	if (speed == 0.0f && !isTaken) {
		this->caller = caller;
		this->callPoint = callPoint;
		callDistanceTravelled = 0.0f;
		pathPoints.clear();
		engine->findPath(position, callPoint, gridMap, width, height, &pathPoints);
		ballType = BALL_TYPE_CALL_BALL;
		return true;
	} else {
		return false;
	}
}

Competitor *BallObject::getCaller() {
	return caller;
}

float BallObject::getRadius() {
	return radius;
}

void BallObject::reverseLookAt() {
	if (!reverseLock) {
		InteractiveObject::reverseLookAt();
	}
	reverseLock = true;
}

void BallObject::addToScore(uint32 value) {
	score += value;
}

uint32 BallObject::getScore() {
	return score;
}

void BallObject::updateGridMap() {
	engine->createGridMap(&gridMap, bBoxes, width, height, false);
}

void BallObject::moveTowardsCallPoint() {
	if (pathPoints.size() == 0) {
		ballType = BALL_TYPE_NORMAL;
	} else {
		Vector2f pathPoint = pathPoints.front();
		float travelDistance = pathPoint.getDistance(position);
		if (travelDistance > 10.0f) {
			travelDistance = 10.0f;
		} else {
			pathPoints.pop_front();
		}
		if (travelDistance == 0) {
			return;
		}

		Vector2f direction = pathPoint - position;
		direction.normalise();
		setPosition(Vector2f(position.x + direction.x * travelDistance, position.y + direction.y * travelDistance));

		callDistanceTravelled += travelDistance;
		if (callDistanceTravelled >= 640.0f) {
			ballType = BALL_TYPE_NORMAL;
		}
	}
}
