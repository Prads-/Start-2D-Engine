#include "Sweeper.h"
#include "SfxPool.h"
using namespace std;

Sweeper::Sweeper(StartEngine *engine, Vector2f position, Image *shadowSprite,
		int initialShadowPeriod, CampaignPlayer *player, BallObject *ball, Grid **gridMap,
		const std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers, 
		std::vector<Sprite*> *sprites, SpritePack *spritePack) 
				 : PathFindingEnemy(engine, bBoxes, 2, ball, gridMap, shadowSprite, initialShadowPeriod, immobilisers, sprites), 
				 coolDownTimer(30), dashPreparationTimer(30), spritePack(*spritePack) {
	width = SWEEPER_WIDTH;
	height = SWEEPER_HEIGHT;
	radius = SWEEPER_WIDTH / 2.0f;
	setPosition(position);
	this->player = player;
	setSpeed(2.5f);
	this->health = health;
	mode = MODE_NORMAL;
}

void Sweeper::update() {
	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
	} else {
		switch (mode) {
			case MODE_NORMAL:
				normalMode();
				break;
			case MODE_DASH_PREPARATION:
				if (updateDashPreparation()) {
					initiateDash();
				} else {
					normalMode();
				}
				break;
			case MODE_DASH:
				if (performDash()) {
					if (position.x < 0 || position.x >= FRAME_WIDTH || position.y < 0 || position.y >= FRAME_HEIGHT || isThereCollision()) {
						mode = MODE_DASH_RETRIEVE;
						Vector2f direction = dashStartPosition - position;
						rotateDegree = direction.getSignedAngleBetweenInDegrees(Vector2f(0.0f, -1.0f));
					} else {
						mode = MODE_COOLDOWN;
						coolDownTimer.startTimer();
					}
				}
				break;
			case MODE_DASH_RETRIEVE:
				performReturnBack();
				break;
			case MODE_COOLDOWN:
				if (coolDownTimer.checkTimer()) {
					mode = MODE_NORMAL;
				}
		}
		checkCollisionWithPlayer();
	}

	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void Sweeper::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), rotateDegree, false, false, false, position.x, position.y);
		Image *effect = 0;
		switch (mode) {
			case MODE_DASH:
				engine->draw(spritePack.spriteChargeStateEffect, rotateDegree, false, false, false, position.x - 8.0f, position.y - 8.0f);
				break;
			case MODE_DASH_PREPARATION:
			case MODE_DASH_RETRIEVE:
				engine->draw(spritePack.spritePreparationStateEffect, rotateDegree, false, false, false, position.x - 4.0f, position.y - 4.0f);
				break;
			default:
				engine->draw(spritePack.spriteBaseBoostEffect, rotateDegree, false, false, false, position.x - 4.0f, position.y - 4.0f);
			case MODE_COOLDOWN:
				engine->draw(spritePack.spriteDefaultStateEffect, rotateDegree, false, false, false, position.x, position.y);
		}
	}

	drawStunEffect();
}

void Sweeper::coolDown() {
	mode = MODE_COOLDOWN;
	coolDownTimer.startTimer();
}

void Sweeper::normalMode() {
	Vector2f playerPosition = player->getPosition();
	previousPosition = position;

	if (checkForObstacles(playerPosition)) {
		findPathTowardsPlayer(playerPosition, 400.0f, &lookAt);
	} else {
		invalidatePathTargetPosition();
		moveTowardsPosition(player->getPosition());
	}
	checkCollisionWithObstacles(lookAt);

	if (mode != MODE_DASH_PREPARATION) {
		float distanceFromPlayer = player->getPosition().getDistanceSquared(position);
		if (distanceFromPlayer <= 22500.0f) {
			prepareToDash();
		}
	}
	calculateRotateFromLook();
}

void Sweeper::initiateDash() {
	dashStartPosition = position;
	dashDirection = player->getPosition() - position;
	dashDirection.normalise();
	if (dashDirection.x == 0.0f && dashDirection.y == 0.0f) {
		dashDirection = lookAt;
	}
	rotateDegree = dashDirection.getSignedAngleBetweenInDegrees(Vector2f(0.0f, -1.0f));
	mode = MODE_DASH;
}

bool Sweeper::performDash() {
	setPosition(Vector2f(position.x + 4.0f * dashDirection.x, position.y + 4.0f * dashDirection.y));
	float distanceToDashStart = dashStartPosition.getDistanceSquared(position);
	if (distanceToDashStart >= 40000.0f) {
		return true;
	}
	return false;
}

void Sweeper::performReturnBack() {
	if (moveTowardsPosition(dashStartPosition)) {
		mode = MODE_COOLDOWN;
		coolDownTimer.startTimer();
	}
}

void Sweeper::drawStunEffect() {
	if (isStunned) {
		const Image *stunEffectFrame;
		stunEffectAnimation.getAnimationFrame(&stunEffectFrame);
		engine->draw(stunEffectFrame, 0, false, false, false, position.x - 13, position.y - 13);
	}
}

void Sweeper::checkCollisionWithPlayer() {
    Vector2f midPoint = getMidPosition(), playerMidPoint = player->getMidPosition();
    if (engine->testCollision(&midPoint, &playerMidPoint, radius, player->getRadius())) {
		player->hurtPlayer();
	}
}

void Sweeper::prepareToDash() {
	mode = MODE_DASH_PREPARATION;
	dashPreparationTimer.startTimer();
	engine->playAudioBuffer(SfxPool::getSfxDetection2());
}

bool Sweeper::updateDashPreparation() {
	return dashPreparationTimer.checkTimer();
}

void Sweeper::makeModeNormal() {
	mode = MODE_NORMAL;
}

bool Sweeper::isDead() const {
	return health <= 0;
}

bool Sweeper::checkDeathTimer() {
	return deathTimer.checkTimer();
}

void Sweeper::pause() {
	Enemy::pause();
	coolDownTimer.pauseTimer();
	dashPreparationTimer.pauseTimer();
}

void Sweeper::resume() {
	Enemy::resume();
	coolDownTimer.resumeTimer();
	dashPreparationTimer.resumeTimer();
}
