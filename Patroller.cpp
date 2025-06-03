#include "Patroller.h"
#include "MT_RandomGenerator.h"
#include <fstream>
#include "SfxPool.h"
using namespace std;

Patroller::Patroller(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, Image *shadowSprite, int health, BallObject *ball, int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, Vector2f position, CampaignPlayer *player, Image *projectileSprite, Patroller::SpritePack *spritePack) 
		:PatrolRover(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites, player, position), confuseTimer(90), spritePack(*spritePack) {
	speed = 2.5f;
	mode = MODE_NORMAL;
	this->projectileSprite = projectileSprite;
	this->spritePack = *spritePack;
	cannonRotateDegrees = 0;
	directionChangeTimer.changeInterval(90);
	cooldownTimer.changeInterval(60);
}

void Patroller::update() {
	if (doPreUpdate()) {
		return;
	}

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
	} else {
		switch (mode) {
			case MODE_NORMAL:
				normalMode();
				break;
			case MODE_ATTACK:
				attackMode();
				break;
			case MODE_CONFUSE:
				confuseMode();
		}
	}
	updateProjectile();
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void Patroller::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), currentRotateDegrees, false, false, false, position.x, position.y);
		engine->draw(spritePack.spriteCanon, cannonRotateDegrees, false, false, false, position.x, position.y);
		if (mode == MODE_ATTACK) {
			engine->draw(spritePack.spriteAttack, cannonRotateDegrees, false, false, false, position.x, position.y);
		}
	}

	drawStunEffect();
}

void Patroller::pause() {
	PatrolRover::pause();
	confuseTimer.pauseTimer();
}

void Patroller::resume() {
	PatrolRover::resume();
	confuseTimer.resumeTimer();
}

void Patroller::normalMode() {
	if (checkCurrentRotation()) {
		move();
		checkChangeDirection();
		if (!checkForObstacles(player->getMidPosition())) {
			previousLookAt = lookAt;
			mode = MODE_ATTACK;
		}
	}
}

void Patroller::attackMode() {
	lookAt = player->getMidPosition() - getMidPosition();
	lookAt.normalise();
	calculateRotateFromLook();
	cannonRotateDegrees = rotateDegree;
	if (isCoolingDown) {
		isCoolingDown = !cooldownTimer.checkTimer();
	} else {
		projectiles.push_back(PatrollerProjectile(engine, bBoxes, player, getMidPosition(), lookAt, 7.0f, projectileSprite));
		isCoolingDown = true;
		cooldownTimer.startTimer();
		engine->playAudioBuffer(SfxPool::getSfxEnemyShoot2());
	}
	if (checkForObstacles(player->getMidPosition())) {
		mode = MODE_CONFUSE;
		confuseTimer.startTimer();
	}
}

void Patroller::confuseMode() {
	if (confuseTimer.checkTimer()) {
		lookAt = previousLookAt;
		calculateRotateFromLook();
		cannonRotateDegrees = currentRotateDegrees;
		mode = MODE_NORMAL;
	} else if (!checkForObstacles(player->getMidPosition())) {
		mode = MODE_ATTACK;
	}
}

void Patroller::updateProjectile() {
	for (list<PatrollerProjectile>::iterator it = projectiles.begin(); it != projectiles.end();) {
		it->update();
		it->draw();
		if (it->isThereCollision()) {
			it = projectiles.erase(it);
		} else {
			++it;
		}
	}
}

bool Patroller::checkCurrentRotation() {
	if (PatrolRover::checkCurrentRotation()) {
		return true;
	} else {
		cannonRotateDegrees = currentRotateDegrees;
		return false;
	}
}