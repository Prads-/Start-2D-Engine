#include "Minion2.h"
#include <fstream>
#include "SfxPool.h"

Minion2::Minion2(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
				 int initialShadowPeriod, std::list<Immobiliser> *immobilisers, float speed, float chasingSpeed, 
				 CampaignPlayer *player, Vector2f position, std::vector<Sprite*> *sprites, Minion2::SpritePack *spritePack)
		: Minion1(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriod, immobilisers, speed, player, position, sprites), hitStunTimer(45), 
		spritePack(*spritePack) {
	mode = MODE_NORMAL;
	this->spriteConfused = spriteConfused;
	this->spriteChasing = spriteChasing;
	this->chasingSpeed = chasingSpeed;
	moveVec = startLookAt;
}

void Minion2::move() {
	updateMode();

	switch (mode) {
		case MODE_NORMAL:
			Minion1::move();
			break;
		case MODE_CHASING:
			lastKnownPlayerPosition = player->getPosition();
			if (!checkCollisionWithPlayer()) {
				Vector2f direction = player->getPosition() - position;
				direction.normalise();
				calculateLookVector(direction);
				setPosition(Vector2f(position.x + moveVec.x * chasingSpeed, position.y + moveVec.y * chasingSpeed));
				moveVec = lookAt;
				Enemy::checkCollisionWithObstacles(moveVec);
			}
			break;
		case MODE_CONFUSED:
			{
				Vector2f direction = lastKnownPlayerPosition - position;
				direction.normalise();
				calculateLookVector(direction);
				setPosition(Vector2f(position.x + moveVec.x * chasingSpeed, position.y + moveVec.y * chasingSpeed));
				moveVec = lookAt;
				Enemy::checkCollisionWithObstacles(moveVec);
			}
			break;
	}
}

void Minion2::updateMode() {
	bool obstacleBetweenPlayer = checkForObstacles(player->getPosition(), width, height);
	switch (mode) {
		case MODE_NORMAL:
			if (!obstacleBetweenPlayer) {
				Vector2f direction = player->getPosition() - position;
				direction.normalise();
				lookAt = direction;
				mode = MODE_CHASING;
				engine->playAudioBuffer(SfxPool::getSfxDetection1());
			}
			break;
		case MODE_CHASING:
			if (obstacleBetweenPlayer) {
				mode = MODE_CONFUSED;
			}
			break;
		case MODE_CONFUSED:
			if (!obstacleBetweenPlayer) {
				mode = MODE_CHASING;
				engine->playAudioBuffer(SfxPool::getSfxDetection1());
			} else if (checkForObstacles(lastKnownPlayerPosition, width, height) || lastKnownPlayerPosition.getDistanceSquared(position) < 25.0f) {
				mode = MODE_NORMAL;
			}
			break;
		default:
			if (hitStunTimer.checkTimer()) {
				mode = MODE_NORMAL;
			}
	}
}

void Minion2::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), 0, false, false, false, position.x, position.y);

		const Image *sprite;
		if (isAttacking) {
			sprite = spritePack.spriteAttackState;
			engine->draw(spritePack.spriteAttackEffect, 0, false, false, false, position.x - 6.0f, position.y - 6.0f);
		} else {
			switch (mode) {
				case MODE_CONFUSED:
					sprite = spritePack.spriteConfusedState;
					drawFacingTowardsPlayer(spritePack.spriteConfusedEffect, lastKnownPlayerPosition);
					break;
				case MODE_CHASING:
					sprite = spritePack.spriteChasingState;
					drawFacingTowardsPlayer(spritePack.spriteSightDetectionEffect, player->getPosition());
					break;
				case MODE_HIT_STUNNED:
					sprite = spritePack.spriteHitStunnedState;
					break;
				default:
					sprite = spritePack.spriteDefaultState;
			}
		}
		engine->draw(sprite, 0, false, false, false, position.x, position.y);
	}

	drawStunEffect();
}

void Minion2::calculateLookVector(Vector2f direction) {
	float angle = direction.getSignedAngleBetween(lookAt);
	if (angle == 0.0f) {
		return;
	}
	angle *= 180.0f / _FLOAT_PI;
	if (angle < 0) {
		if (angle < -2.0f) {
			lookAt.rotate(engine->sin(2), engine->cos(2));
		} else {
			lookAt = direction;
		}
	} else {
		if (angle > 2.0f) {
			int turnAngle = 360 - 2;
			lookAt.rotate(engine->sin(turnAngle), engine->cos(turnAngle));
		} else {
			lookAt = direction;
		}
	}
}

void Minion2::drawFacingTowardsPlayer(const Image *sprite, Vector2f faceTowards) {
	Vector2f towardsPlayer = faceTowards - getMidPosition();
	int angle = towardsPlayer.getSignedAngleBetweenInDegrees(Vector2f(0.0f, -1.0f));
	engine->draw(sprite, angle, false, false, false, position.x - 15.0f, position.y - 15.0f);
}

void Minion2::checkCollisionWithBall() {
	Enemy::checkCollisionWithBall();
	if (isImmune && mode != MODE_HIT_STUNNED) {
		mode = MODE_HIT_STUNNED;
		hitStunTimer.startTimer();
	}
}

void Minion2::pause() {
	Minion1::pause();
	hitStunTimer.pauseTimer();
}

void Minion2::resume() {
	Minion1::resume();
	hitStunTimer.resumeTimer();
}