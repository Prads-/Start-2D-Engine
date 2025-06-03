#include "Enemy.h"
#include "Explosion.h"
#include <fstream>
#include <vector>
#include "EnemyManager.h"
#include "MT_RandomGenerator.h"
#include "SfxPool.h"
#include "AbilityCoolDownValues.h"
using namespace RayIntersection;
using namespace std;

Animation Enemy::spawnEffect("Images/enemies/Commons/SpawnEffect.anim", true);
Animation Enemy::deathExplosionEffect("Images/enemies/Commons/DeathExplosion.anim", true);
Animation Enemy::stunEffectAnimationFrames("Images/Stun.anim");

Enemy::Enemy(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites) 
		: InteractiveObject(engine, bBoxes), shadowPeriodTimer(initialShadowPeriodInterval), stunnedTimer(180), deathTimer(90) {
	this->health = health;
	this->ball = ball;
	this->shadowSprite = shadowSprite;
	this->isInInitialShadowPeriod = initialShadowPeriodInterval;
	this->immobilisers = immobilisers;
	this->sprites = sprites;
	shadowPeriodTimer.startTimer();
	isImmune = false;
	setStartLook(Vector2f(0.0f, -1.0f));
	isStunned = false;
	radius = 0.0f;
	if (sprites) {
		updateSpriteCounter();
	}

	stunEffectAnimation.linkAnimationFrames(&stunEffectAnimationFrames);
	stunEffectAnimation.changeAnimationInterval(stunnedTimer.getInterval() / stunEffectAnimation.getNumberOfFrames());
	stunEffectAnimation.setLoop(false);
}

void Enemy::collisionResponse(const BoundingBox *bBox, Vector2f &direction) {
	CollisionSide collisionSide = getCollisionSide(bBox);
	switch (collisionSide) {
		case TOP:
			if (direction.y < 0.0f) {
				direction.y = 0.0f;
				if (direction.x != 0.0f) {
					(direction.x < 0.0f) ? direction.x = -1.0f : direction.x = 1.0f;
				}
			}
			position.y = previousPosition.y;
			calculateBB();
			break;
		case BOTTOM:
			if (direction.y > 0.0f) {
				direction.y = 0.0f;
				if (direction.x != 0.0f) {
					(direction.x < 0.0f) ? direction.x = -1.0f : direction.x = 1.0f;
				}
			}
			position.y = previousPosition.y;
			calculateBB();
			break;
		case LEFT:
			if (direction.x < 0.0f) {
				direction.x = 0.0f;
				if (direction.y != 0.0f) {
					(direction.y < 0.0f) ? direction.y = -1.0f : direction.y = 1.0f;
				}
			}
			position.x = previousPosition.x;
			calculateBB();
			break;
		case RIGHT:
			if (direction.x > 0.0f) {
				direction.x = 0.0f;
				if (direction.y != 0.0f) {
					(direction.y < 0.0f) ? direction.y = -1.0f : direction.y = 1.0f;
				}
			}
			position.x = previousPosition.x;
			calculateBB();
	}
}

bool Enemy::checkForObstacles(Vector2f to, int rightLength, int bottomLength) {
	RayIntersection::Line aiToTarget1(position, to);
	RayIntersection::Line aiToTarget2(Vector2f(position.x + rightLength, position.y), Vector2f(to.x + rightLength, to.y));
	RayIntersection::Line aiToTarget3(Vector2f(position.x + rightLength, position.y + bottomLength), Vector2f(to.x + rightLength, to.y + bottomLength));
	RayIntersection::Line aiToTarget4(Vector2f(position.x, position.y + bottomLength), Vector2f(to.x, to.y + bottomLength));

	for (size_t i = 4; i < bBoxes->size(); ++i) {
		if (engine->checkIfLineSegmentIntersectsBB(&aiToTarget1, &(*bBoxes)[i]) || engine->checkIfLineSegmentIntersectsBB(&aiToTarget2, &(*bBoxes)[i]) ||
			engine->checkIfLineSegmentIntersectsBB(&aiToTarget3, &(*bBoxes)[i]) || engine->checkIfLineSegmentIntersectsBB(&aiToTarget4, &(*bBoxes)[i])) {
			return true;
		}
	}

	return false;
}

bool Enemy::checkForObstacles(Vector2f to) {
	RayIntersection::Line aiToTarget1(position, to);
	RayIntersection::Line aiToTarget2(Vector2f(position.x + width, position.y), to);
	RayIntersection::Line aiToTarget3(Vector2f(position.x + width, position.y + height), to);
	RayIntersection::Line aiToTarget4(Vector2f(position.x, position.y + height), to);

	for (size_t i = 4; i < bBoxes->size(); ++i) {
		if (engine->checkIfLineSegmentIntersectsBB(&aiToTarget1, &(*bBoxes)[i]) || engine->checkIfLineSegmentIntersectsBB(&aiToTarget2, &(*bBoxes)[i]) ||
			engine->checkIfLineSegmentIntersectsBB(&aiToTarget3, &(*bBoxes)[i]) || engine->checkIfLineSegmentIntersectsBB(&aiToTarget4, &(*bBoxes)[i])) {
			return true;
		}
	}

	return false;
}

bool Enemy::isDead() const {
	return health <= -1;
}

bool Enemy::isInDeathPeriod() const {
	return health == 0;
}

void Enemy::updateDeathPeriod() {
	if (deathTimer.checkTimer()) {
		health = -1;
	}
}

void Enemy::checkCollisionWithBall() {
	bool wasCollision = false;
	if (ball->getSpeed() > BALL_CATCHABLE_SPEED || ball->getBallType() == BALL_TYPE_CALL_BALL) {
		if (testCollisionWithBall()) {
			if (!isImmune) {
				takeAwayHP();
				ball->setHitTimer();
			}
			wasCollision = true;
		}
	} else {
		Explosion *explosion = Explosion::getInstance();
		if (explosion->getIsActive() && testCollisionWithExplosion()) {
			if (!isImmune) {
				takeAwayHP();
			}
			wasCollision = true;
		}
	}
	isImmune = wasCollision;
}

bool Enemy::getIsInInitialShadowPeriod() const {
	return isInInitialShadowPeriod;
}

void Enemy::pause() {
	shadowPeriodTimer.pauseTimer();
	stunnedTimer.pauseTimer();
	deathTimer.pauseTimer();
}

void Enemy::resume() {
	shadowPeriodTimer.resumeTimer();
	stunnedTimer.resumeTimer();
	deathTimer.resumeTimer();
}

Animation *Enemy::getSpawnEffect() {
	return &spawnEffect;
}

Animation *Enemy::getDeathExplosionEffect() {
	return &deathExplosionEffect;
}

Animation *Enemy::getStunnedEffect() {
	return &stunEffectAnimationFrames;
}

bool Enemy::checkShadowPeriod() {
	if (isInInitialShadowPeriod) {
		if (shadowPeriodTimer.checkTimer()) {
			isInInitialShadowPeriod = false;
		}
	}
	return isInInitialShadowPeriod;
}

void Enemy::checkCollisionWithImmobilisers() {
	for (const Immobiliser &immobiliser : *immobilisers) {
		if (immobiliser.getThrower() != (void*)this && engine->checkCollision(immobiliser.getBoundingBox(), &bBox)) {
			isStunned = true;
			stunnedTimer.startTimer();
			stunEffectAnimation.resetAnimation();
			break;
		}
	}
}

void Enemy::updateStunnedPeriod() {
	isStunned = !stunnedTimer.checkTimer();
}

bool Enemy::getIsStunned() const {
	return isStunned;
}

int Enemy::getHealth() const {
	return health;
}

float Enemy::getRadius() const {
	return radius;
}

void Enemy::setHealth(int health) {
	this->health = health;
}

bool Enemy::checkCollisionWithObstacles(Vector2f &direction) {
	bool wasCollision = false;
	for (size_t i = 0; i < bBoxes->size(); ++i) {
		if (engine->checkCollision(&bBox, &(*bBoxes)[i])) {
			collisionResponse(&(*bBoxes)[i], direction);
			i = -1;
			wasCollision = true;
		}
	}
	return wasCollision;
}

void Enemy::updateSpriteCounter() {
	if (health == 0) {
		return;
	}
	spriteCounter = health - 1;
	if (spriteCounter >= sprites->size()) {
		spriteCounter = sprites->size() - 1;
	}
	while ((*sprites)[spriteCounter] == 0 && spriteCounter != 0) {
		--spriteCounter;
	}
}

void Enemy::takeAwayHP(int hp) {
	health -= hp;
	updateSpriteCounter();
	if (health <= 0) {
		health = 0;
		deathTimer.startTimer();
		engine->playAudioBuffer(SfxPool::getSfxEnemyDeath());
	}
}

bool Enemy::doPreUpdate() {
	if (checkShadowPeriod()) {
		return true;
	} else if (isInDeathPeriod()) {
		updateDeathPeriod();
		return true;
	}
	return false;
}

bool Enemy::testCollisionWithBall() {
    Vector2f midPoint = getMidPosition(), ballMidPoint = ball->getMidPosition();
    return engine->testCollision(&midPoint, &ballMidPoint, radius, ball->getRadius());
}

bool Enemy::testCollisionWithExplosion() {
    Explosion *explosion = Explosion::getInstance();
    Vector2f midPoint = getMidPosition(), explosionMidPoint = explosion->getMidPosition();
    return engine->testCollision(&midPoint, &explosionMidPoint, radius, explosion->getRadius());
}

Vector2f Enemy::getClosestIntersectionPointAgainstObstacle(Line line) {
	vector<Vector2f> intersectionPoints;

	for (const BoundingBox &bBox : *bBoxes) {
		Line bbEdge[4];
		bbEdge[0].p1 = bBox.minP;
		bbEdge[0].p2 = Vector2f(bBox.maxP.x, bBox.minP.y);
		bbEdge[1].p1 = bbEdge[0].p2;
		bbEdge[1].p2 = bBox.maxP;
		bbEdge[2].p1 = bBox.maxP;
		bbEdge[2].p2 = Vector2f(bBox.minP.x, bBox.maxP.y);
		bbEdge[3].p1 = bbEdge[2].p2;
		bbEdge[3].p2 = bBox.minP;
		Vector2f lineIntersectionPoint;
		for (int i = 0; i < 4; ++i) {
			if (getLineIntersectionPoint(&line, &bbEdge[i], &lineIntersectionPoint)) {
				intersectionPoints.push_back(lineIntersectionPoint);
			}
		}
	}

	Vector2f closestIntersectionPoint = intersectionPoints[0];
	float squaredDistance = closestIntersectionPoint.getDistanceSquared(line.p1);
	for (int i = 1; i != intersectionPoints.size(); ++i) {
		float currentSquaredDistance = intersectionPoints[i].getDistanceSquared(line.p1);
		if (currentSquaredDistance < squaredDistance) {
			closestIntersectionPoint = intersectionPoints[i];
			squaredDistance = currentSquaredDistance;
		}
	}

	return closestIntersectionPoint;
}

bool Enemy::checkIfManagerIsInList(EnemyManager *enemyManager, std::list<EnemyManager*> *enemyManagers) {
	for (EnemyManager *manager : *enemyManagers) {
		if (enemyManager == manager) {
			return true;
		}
	}
	return false;
}

void Enemy::drawShadowPeriod() {
	const Image *spawnEffectFrame;
	spawnEffect.getAnimationFrame(&spawnEffectFrame);
	Vector2f midPosition = getMidPosition();
	engine->draw(spawnEffectFrame, 0, false, false, false, midPosition.x - spawnEffectFrame->getWidth() / 2, midPosition.y - spawnEffectFrame->getHeight() / 2);
	engine->draw(shadowSprite, rotateDegree, false, false, true, position.x, position.y);
}

void Enemy::drawDeathPeriod(const Image *deathSprite) {
	engine->draw(deathSprite, rotateDegree, false, false, false, position.x, position.y);
	const Image *deathExplosionFrame;
	deathExplosionEffect.getAnimationFrame(&deathExplosionFrame);
	Vector2f midPosition = getMidPosition();
	engine->draw(deathExplosionFrame, 0, false, false, true, midPosition.x - deathExplosionFrame->getWidth() / 2, midPosition.y - deathExplosionFrame->getHeight() / 2);
}

void Enemy::drawStunEffect() {
	if (isStunned) {
		const Image *stunEffectFrame;
		stunEffectAnimation.getAnimationFrame(&stunEffectFrame);
		engine->draw(stunEffectFrame, 0, false, false, false, position.x - 3, position.y - 3);
	}
}