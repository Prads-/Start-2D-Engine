#include "Minion1.h"
#include "MT_RandomGenerator.h"
#include <fstream>

Minion1::Minion1(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
				 int initialShadowPeriod, std::list<Immobiliser> *immobilisers, float speed, CampaignPlayer *player, 
				 Vector2f position, std::vector<Sprite*> *sprites, SpritePack *spritePack) 
		: Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriod, immobilisers, sprites), directionChangeTimer(300),
		spritePack(*spritePack) {
	commonInit(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriod, immobilisers, speed, player, position, sprites);
}

Minion1::Minion1(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriod, std::list<Immobiliser> *immobilisers, float speed, CampaignPlayer *player, 
		Vector2f position, std::vector<Sprite*> *sprites) : Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriod, immobilisers, sprites), directionChangeTimer(300), spritePack(0, 0) {
	commonInit(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriod, immobilisers, speed, player, position, sprites);
}

void Minion1::commonInit(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriod, std::list<Immobiliser> *immobilisers, float speed, CampaignPlayer *player, 
		Vector2f position, std::vector<Sprite*> *sprites) {
	width = 30;
	height = 30;
	radius = 30.0f / 2.0f;
	this->speed = speed;
	this->player = player;
	rotateDegree = MT_RandomGenerator::randomNumber() % 360;
	calculateLookFromRotateDegree();
	getRandomAngle();
	setPosition(position);
	isAttacking = false;
}

void Minion1::update() {
	if (doPreUpdate()) {
		return;
	}

	if (isStunned) {
		updateStunnedPeriod();
	} else {
		previousPosition = position;
		move();
	}
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void Minion1::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
	}else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), 0, false, false, false, position.x, position.y);
		if (isAttacking) {
			engine->draw(spritePack.spriteAttack, 0, false, false, false, position.x - 2.0f, position.y - 2.0f);
		}		
	}

	drawStunEffect();
}

void Minion1::getRandomAngle() {
	randomAngle = MT_RandomGenerator::randomNumber() % 46;
}

void Minion1::checkCollisionWithObstacles() {
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, &bBox)) {
			collisionWithObstacle(&obstacle);
			position = previousPosition;
			calculateBB();
			break;
		}
	}
}

void Minion1::collisionWithObstacle(const BoundingBox *bBox) {
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
}

bool Minion1::checkCollisionWithPlayer() {
    Vector2f midPoint = getMidPosition(), playerMidPoint = player->getMidPosition();
    if (engine->testCollision(&midPoint, &playerMidPoint, radius, player->getRadius())) {
		player->hurtPlayer();
		isAttacking = true;
		return true;
	}
	isAttacking = false;
	return false;
}

void Minion1::pause() {
	Enemy::pause();
	directionChangeTimer.pauseTimer();
}

void Minion1::resume() {
	Enemy::resume();
	directionChangeTimer.resumeTimer();
}

void Minion1::move() {
	setPosition(Vector2f(position.x + lookAt.x * speed, position.y + lookAt.y * speed));
	checkCollisionWithObstacles();
	checkCollisionWithPlayer();
	if (directionChangeTimer.checkTimer()) {
		getRandomAngle();
	}
}

void Minion1::drawStunEffect() {
	if (isStunned) {
		const Image *stunEffectFrame;
		stunEffectAnimation.getAnimationFrame(&stunEffectFrame);
		engine->draw(stunEffectFrame, 0, false, false, false, position.x - 8, position.y - 8);
	}
}