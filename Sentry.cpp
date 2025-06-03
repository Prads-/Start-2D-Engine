#include "Sentry.h"
#include "RayIntersection.h"
#include <vector>
#include <fstream>
#include "SfxPool.h"
using namespace RayIntersection;
using namespace std;

Sentry::Sentry(StartEngine *engine, Image *shadowSprite, int initialShadowPeriod, const std::vector<BoundingBox> *bBoxes, 
			   Vector2f startPosition, CampaignPlayer *player, BallObject *ball, std::list<Immobiliser> *immobilisers, 
			   std::vector<Sprite*> *sprites, SpritePack *spritePack) : 
		Enemy(engine, bBoxes, 1, ball, shadowSprite, initialShadowPeriod, immobilisers, sprites), coolDownTimer(30),
		spritePack(*spritePack) {
	width = 40;
	height = 40;
	radius = 40.0f / 2.0f;
	this->player = player;
	setPosition(startPosition);
	isCoolingDown = false;
}

void Sentry::update() {
	if (doPreUpdate()) {
		return;
	}
	
	if (isStunned) {
		updateStunnedPeriod();
	} else {
		Vector2f midPosition = getMidPosition();
		Vector2f towardsPlayer = player->getMidPosition() - midPosition;
		towardsPlayer.normalise();
		setLook(towardsPlayer);

		if (isCoolingDown) {
			isCoolingDown = !coolDownTimer.checkTimer();
		} else {
			if (!checkForObstacles(player->getMidPosition())) {
				SentryProjectile projectile = SentryProjectile(engine, spritePack.spriteProjectile, bBoxes, player, 18.0f);
				projectile.setPosition(midPosition);
				projectile.setLook(towardsPlayer);
				projectiles.push_back(projectile);
				isCoolingDown = true;
				coolDownTimer.startTimer();
				engine->playAudioBuffer(SfxPool::getSfxEnemyShoot2());
			}
		}
	}

	updateProjectile();
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void Sentry::updateProjectile() {
	for (list<SentryProjectile>::iterator it = projectiles.begin(); it != projectiles.end(); ) {
		if (it->isThereCollision()) {
			it = projectiles.erase(it);
		} else {
			it->update();
			++it;
		}
	}
}

void Sentry::drawProjectiles() {
	for (SentryProjectile &projectile : projectiles) {
		projectile.draw();
	}
}

void Sentry::drawTargetLine() {
	Vector2f midPosition = getMidPosition();
	Vector2f rightVector = Vector2f(-lookAt.y, lookAt.x);
	midPosition.x += lookAt.x * width / 2 + rightVector.x * 2.0f;
	midPosition.y += lookAt.y * height / 2 + rightVector.y * 2.0f;
	Line targetLine(midPosition, Vector2f(midPosition.x + lookAt.x * 2000.0f, midPosition.y + lookAt.y * 2000.0f));
	Vector2f closestPoint = getClosestIntersectionPointAgainstObstacle(targetLine);
	uint32 widthColors[5] = {0x9FFFFF00, 0x8FFF7F00, 0x7FFF0000, 0x8FFF7F00, 0x9FFFFF00};
	engine->drawThickBeamNoBoundCheck(midPosition, lookAt, midPosition.getDistance(closestPoint), widthColors);
}

void Sentry::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
	} else {
		drawTargetLine();
		drawProjectiles();
		const Image *sprite;
		sprite = (*sprites)[spriteCounter]->getSprite();
		engine->draw(sprite, 0, false, false, false, position.x, position.y);
		engine->draw(spritePack.spriteCannon, rotateDegree, false, false, false, position.x, position.y);
	}

	drawStunEffect();
}

void Sentry::pause() {
	Enemy::pause();
	coolDownTimer.pauseTimer();
}

void Sentry::resume() {
	Enemy::resume();
	coolDownTimer.resumeTimer();
}