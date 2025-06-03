#include "MB01Sensor.h"

MB01Sensor::MB01Sensor(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
			int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, 
			Vector2f position, const Image *deathSprite)
			:PatrolRover(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites, player, position) {
	width = 20;
	height = 20;
	speed = 3.0f;
	hasSeenPlayer = false;
	this->deathSprite = deathSprite;
}

bool MB01Sensor::getHasSeenPlayer() const {
	return hasSeenPlayer;
}

void MB01Sensor::update() {
	if (isInDeathPeriod()) {
		updateDeathPeriod();
		return;
	}

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
		hasSeenPlayer = false;
	} else {
		if (checkForObstacles(player->getPosition(), width, height)) {
			hasSeenPlayer = false;
			if (checkCurrentRotation()) {
				move();
				checkChangeDirection();
			}
		} else {
			hasSeenPlayer = true;
			Vector2f direction = player->getPosition() - position;
			currentRotateDegrees = direction.getSignedAngleBetweenInDegrees(Vector2f(0.0f, -1.0f));
		}
	}
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void MB01Sensor::draw() {
	if (isInDeathPeriod()) {
		drawDeathPeriod(deathSprite);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), 0, false, false, false, position.x, position.y);
		if (hasSeenPlayer && !isStunned) {
			Vector2f midPosition = getMidPosition(), playerMidPosition = player->getMidPosition();
			Vector2f lineDirection = playerMidPosition - midPosition;
			lineDirection.normalise();
			engine->drawLine(midPosition, lineDirection, midPosition.getDistance(playerMidPosition) - player->getWidth() / 2, 0xFFFF00);
		}
	}

	drawStunEffect();
}

void MB01Sensor::drawStunEffect() {
	if (isStunned) {
		const Image *stunEffectFrame;
		stunEffectAnimation.getAnimationFrame(&stunEffectFrame);
		engine->draw(stunEffectFrame, 0, false, false, false, position.x - 13, position.y - 13);
	}
}