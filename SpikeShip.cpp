#include "SpikeShip.h"
#include <fstream>

SpikeShip::SpikeShip(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
					 int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player,
					 const Image *deathSprite)
		: Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites) {
	width = 40;
	height = 40;
	radius = width / 2;
	rotateSpeed = 10;
	mode = MODE_NORMAL;
	cooldownTimer.changeInterval(120);
	speed = 20.0f;
	this->player = player;
	this->deathSprite = deathSprite;
}

void SpikeShip::update() {
	if (doPreUpdate()) {
		return;
	}

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
	} else {
		previousPosition = position;
		switch (mode) {
			case MODE_NORMAL:
				modeNormal();
				break;
			case MODE_CHASE:
				modeChase();
				break;
			case MODE_COOLDOWN:
				modeCooldown();
		}
	}
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
	checkCollisionWithPlayer();
}

void SpikeShip::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(deathSprite);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), rotateDegree, false, false, false, position.x, position.y);
	}

	drawStunEffect();
}

void SpikeShip::modeNormal() {
	if (!checkForObstacles(player->getPosition(), width, height)) {
		lookAt = player->getPosition() - position;
		lookAt.normalise();
		mode = MODE_CHASE;
	}
}

void SpikeShip::modeChase() {
	rotateDegree += rotateSpeed;
	if (rotateDegree >= 360) {
		rotateDegree = 360 - rotateDegree;
	}
	setPosition(Vector2f(position.x + lookAt.x * speed, position.y + lookAt.y * speed));
	if (isThereCollision()) {
		setPosition(previousPosition);
		rotateDegree = 0;
		mode = MODE_COOLDOWN;
		cooldownTimer.startTimer();
	}
}

void SpikeShip::modeCooldown() {
	if (cooldownTimer.checkTimer()) {
		mode = MODE_NORMAL;
	}
}

void SpikeShip::checkCollisionWithPlayer() {
	if (engine->checkCollision(player->getBoundingBox(), &bBox)) {
		player->hurtPlayer();
	}
}

void SpikeShip::pause() {
	Enemy::pause();
	cooldownTimer.pauseTimer();
}

void SpikeShip::resume() {
	Enemy::resume();
	cooldownTimer.resumeTimer();
}