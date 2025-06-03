#include "Dragoon.h"
#include <fstream>
#include "Explosion.h"
#include "MT_RandomGenerator.h"
#include "SfxPool.h"
using namespace std;

#define DRAGOON_SHOOTING_RANGE			300.0f
#define DRAGOON_STOP_RANGE				250.0f

Dragoon::Dragoon(StartEngine *engine, Image *projectileSprite, Image *shadowSprite, int initialShadowPeriod, 
				 const std::vector<BoundingBox> *bBoxes, BallObject *ball, Grid** gridMap, 
				 CampaignPlayer *player, Vector2f position, std::list<Immobiliser> *immobilisers, 
				 const std::list<void*> *dragoons, std::vector<Sprite*> *sprites, SpritePack *spritePack, const Animation *shieldRecovery)
		: PathFindingEnemy(engine, bBoxes, health, ball, gridMap, shadowSprite, initialShadowPeriod, immobilisers, sprites), 
		hitStunnedTimer(90), coolDownTimer(90), spritePack(*spritePack) {
	width = DRAGOON_WIDTH;
	height = DRAGOON_HEIGHT;
	radius = DRAGOON_WIDTH / 2.0f;
	this->projectileSprite = projectileSprite;
	this->player = player;
	this->dragoons = dragoons;
	speed = 3.0f;
	health = 2;
	updateSpriteCounter();
	this->shieldRecovery.linkAnimationFrames(shieldRecovery);
	this->shieldRecovery.changeAnimationInterval(shieldRecovery->getAnimationInterval());

	setStartLook(Vector2f(0.0f, -1.0f));
	moveVec = startLookAt;
	targetPosition = Vector2f(0.0f, 0.0f);
	setPosition(position);
	isHitStunned = false;
	isCoolingDown = false;
	isWaitingForOtherDragoonToMove = false;
	isShootingAtPlayer = false;
}

Dragoon::~Dragoon() {
	if (gridMap) {
		engine->destroyGridMap(gridMap, DRAGOON_WIDTH);
	}
}

void Dragoon::update() {
	if (doPreUpdate()) {
		return;
	}

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
	} else {
		previousPosition = position;

		if (isHitStunned) {
			if (!isDead() && hitStunnedTimer.checkTimer()) {
				isHitStunned = false;
				health = 2;
				updateSpriteCounter();
			}
		} else {
			takeAction();
		}
		updateCoolDownTimer();
	}
	updateProjectiles();
	checkCollision();
}

void Dragoon::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
		return;
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
		return;
	}
	
	engine->draw((*sprites)[spriteCounter]->getSprite(), rotateDegree, false, false, false, position.x, position.y);
	const Image *spriteEffect = 0;
	if (isHitStunned) {
		shieldRecovery.getAnimationFrame(&spriteEffect);
		engine->draw(spriteEffect, 0, false, false, false, position.x, position.y);
		spriteEffect = spritePack.spriteRecoveryState;
	} else if (isShootingAtPlayer) {
		spriteEffect = spritePack.spriteAttackState;
		engine->draw(spritePack.spriteShield, 0, false, false, false, position.x, position.y);
	} else {
		spriteEffect = spritePack.spriteDefaultState;
		engine->draw(spritePack.spriteShield, 0, false, false, false, position.x, position.y);
	}
	engine->draw(spriteEffect, rotateDegree, false, false, false, position.x, position.y);
	drawStunEffect();
}

void Dragoon::takeAction() {
	lookAt = player->getMidPosition() - getMidPosition();
	lookAt.normalise();
	calculateRotateFromLook();

	if (checkForObstacles(player->getMidPosition())) {
		findPathTowardsPlayer(player->getPosition(), 1600.0f, &moveVec);
		isShootingAtPlayer = false;
	} else {
		goStraightToPlayer();
	}
}

void Dragoon::goStraightToPlayer() {
	invalidatePathTargetPosition();
	float distance = position.getDistanceSquared(player->getPosition());
	if (distance < 102400.0f) {
		if (!isCoolingDown) {
			shootTowardsPlayer();
			isCoolingDown = true;
			coolDownTimer.startTimer();
		}
	} else if (distance > 25600.0f) {
		setPosition(Vector2f(position.x + moveVec.x * speed, position.y + moveVec.y * speed));
		moveVec = lookAt;
		isShootingAtPlayer = false;
	}
}

void Dragoon::shootTowardsPlayer() {
	projectileList.push_back(DragoonProjectile(engine, projectileSprite, bBoxes, player, getMidPosition(), lookAt, 5.0f));
	isShootingAtPlayer = true;
	engine->playAudioBuffer(SfxPool::getSfxEnemyShoot2());
}

void Dragoon::updateCoolDownTimer() {
	if (isCoolingDown) {
		if (coolDownTimer.checkTimer()) {
			isCoolingDown = false;
		}
	}
}

void Dragoon::checkCollisionWithBall() {
	bool wasCollision = false;
	if (ball->getSpeed() > BALL_CATCHABLE_SPEED || ball->getBallType() == BALL_TYPE_CALL_BALL) {
		if (testCollisionWithBall()) {
			ballCollisionResponse();
			wasCollision = true;
		}
	} else {
		Explosion *explosion = Explosion::getInstance();
		if (explosion->getIsActive() && testCollisionWithExplosion()) {
			ballCollisionResponse();
			wasCollision = true;
		}
	}
	isImmune = wasCollision;
}

void Dragoon::updateProjectiles() {
	for (list<DragoonProjectile>::iterator it = projectileList.begin(); it != projectileList.end();) {
		it->update();
		it->draw();
		float distance = it->getStartPosition().getDistanceSquared(it->getPosition());
		if (distance >= 102400.0f) {
			it = projectileList.erase(it);
		} else {
			if (it->isThereCollision()) {
				it = projectileList.erase(it);
			} else {
				++it;
			}
		}
	}
}

void Dragoon::ballCollisionResponse() {
	if (!isImmune) {
		takeAwayHP();
		isHitStunned = true;
		hitStunnedTimer.startTimer();
		ball->setHitTimer();
		shieldRecovery.resetAnimation();
	}
}

bool Dragoon::checkCollisionWithOtherDragoons() {
	for (const void *dragoon : *dragoons) {
		if (dragoon != this && !((Dragoon*)dragoon)->getIsWaitingForOtherDragoonToMove()) {
			if (engine->checkCollision(((Dragoon*)dragoon)->getBoundingBox(), &bBox)) {
				isWaitingForOtherDragoonToMove = true;
				position = previousPosition;
				return true;
			}
		}
	}
	return false;
}

void Dragoon::checkCollision() {
	isWaitingForOtherDragoonToMove = false;
	if (!checkCollisionWithOtherDragoons()) {
		checkCollisionWithObstacles(moveVec);
	}
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

bool Dragoon::getIsWaitingForOtherDragoonToMove() const {
	return isWaitingForOtherDragoonToMove;
}

void Dragoon::pause() {
	Enemy::pause();
	hitStunnedTimer.pauseTimer();
	coolDownTimer.pauseTimer();
	shieldRecovery.pause();
}

void Dragoon::resume() {
	Enemy::resume();
	hitStunnedTimer.resumeTimer();
	coolDownTimer.resumeTimer();
	shieldRecovery.resume();
}