#include "TheEye.h"
#include <fstream>
#include "SfxPool.h"
using namespace std;

TheEye::TheEye(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
			   int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player,
			   Image *projectileSprite, const Image *deathSprite)
		:Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites) {
	width = 40;
	height = 40;
	radius = width / 2.0f;
	rotateDegree = 0;
	lookAt = Vector2f(0.0f, -1.0f);
	mode = MODE_ROTATION;
	cooldownDirectShoot.changeInterval(6);
	cooldownRotationShoot.changeInterval(60);
	rotateShootIsCoolingDown = false;
	this->player = player;
	this->projectileSprite = projectileSprite;
	this->deathSprite = deathSprite;
}

void TheEye::update() {
	if (doPreUpdate()) {
		return;
	}

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
	} else {
		if (mode == MODE_ROTATION) {
			rotationMode();
		} else {
			directShootMode();
		}
	}
	updateProjectiles();
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void TheEye::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(deathSprite);
	} else {
		if (!isInDeathPeriod()) { 
			drawProjectiles();
		}
		engine->draw((*sprites)[spriteCounter]->getSprite(), rotateDegree, false, false, false, position.x, position.y);
	}

	drawStunEffect();
}

void TheEye::rotationMode() {
	rotateDegree += 1;
	if (rotateDegree >= 360) {
		rotateDegree -= 360;
	}
	calculateLookFromRotateDegree();
	if (!rotateShootIsCoolingDown) {
		Vector2f towardsPlayer = player->getMidPosition() - getMidPosition();
		towardsPlayer.normalise();
		if (lookAt.getAngleBetween(towardsPlayer) <= 0.087f) {
			towardsPlayer.normalise();
			shootAtPlayer(6.0f, towardsPlayer);
			rotateShootIsCoolingDown = true;
			cooldownRotationShoot.startTimer();
		}
	} else {
		updateRotateShootCoolDown();
	}
	if (!checkForObstacles(player->getMidPosition())) {
		mode = MODE_DIRECT_SHOOT;
	}
}

void TheEye::directShootMode() {
	if (checkForObstacles(player->getMidPosition())) {
		mode = MODE_ROTATION;
	} else {
		lookAt = player->getMidPosition() - getMidPosition();
		lookAt.normalise();
		calculateRotateFromLook();
		if (cooldownDirectShoot.checkTimer()) {
			shootAtPlayer(15.0f, lookAt);
		}
	}
}

void TheEye::updateProjectiles() {
	for (list<TheEyeProjectile>::iterator it = projectiles.begin(); it != projectiles.end();) {
		it->update();
		if (it->getIsActive()) {
			++it;
		} else {
			it = projectiles.erase(it);
		}
	}
}

void TheEye::drawProjectiles() {
	for (TheEyeProjectile &projectile : projectiles) {
		projectile.draw();
	}
}

void TheEye::shootAtPlayer(float projectileSpeed, Vector2f direction) {
	projectiles.push_back(TheEyeProjectile(engine, player, direction, projectileSpeed, getMidPosition(), projectileSprite));
	engine->playAudioBuffer(SfxPool::getSfxEnemyShoot1());
}

void TheEye::updateRotateShootCoolDown() {
	rotateShootIsCoolingDown = !cooldownRotationShoot.checkTimer();
}

void TheEye::pause() {
	Enemy::pause();
	cooldownRotationShoot.pauseTimer();
	cooldownDirectShoot.pauseTimer();
}

void TheEye::resume() {
	Enemy::resume();
	cooldownRotationShoot.resumeTimer();
	cooldownDirectShoot.resumeTimer();
}