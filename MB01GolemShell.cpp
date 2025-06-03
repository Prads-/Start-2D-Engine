#include "MB01GolemShell.h"

MB01GolemShell::MB01GolemShell(StartEngine *engine, CampaignPlayer *player, 
							   int explosionInterval, int explodeInterval, 
							   float speed, Vector2f position, Vector2f lookAt, const std::vector<BoundingBox> *bBoxes, int explosionRadius,
							   bool explodeWhenCollisionWithPlayer, const Animation *shellExplosionAnimation, const Animation *shellAnimation) 
							   :InteractiveObject(engine, bBoxes) {
	width = 10;
	height = 10;

	this->shellAnimation.linkAnimationFrames(shellAnimation);
	this->shellAnimation.changeAnimationInterval(shellAnimation->getAnimationInterval());
	this->shellAnimation.resetAnimation();

	this->shellExplosionAnimation.linkAnimationFrames(shellExplosionAnimation);
	this->shellExplosionAnimation.changeAnimationInterval(shellExplosionAnimation->getAnimationInterval());

	this->player = player;
	explosionTimer.changeInterval(explosionInterval);
	explodeTimer.changeInterval(explodeInterval);
	this->speed = speed;
	setPosition(position);
	explodeTimer.startTimer();
	mode = MODE_NORMAL;
	this->explosionRadius = explosionRadius;
	this->lookAt = lookAt;
	this->explodeWhenCollisionWithPlayer = explodeWhenCollisionWithPlayer;
}

void MB01GolemShell::update() {
	switch (mode) {
		case MODE_NORMAL:
			normalMode();
			break;
		case MODE_EXPLOSION:
			explosionMode();
	}
}

void MB01GolemShell::draw() {
	const Image *sprite;
	
	if (mode == MODE_NORMAL) {
		shellAnimation.getAnimationFrame(&sprite);
		engine->draw(sprite, 0, false, false, false, position.x - sprite->getWidth() / 2, position.y - sprite->getHeight() / 2);
	} else {
		shellExplosionAnimation.getAnimationFrame(&sprite);
		engine->draw(sprite, 0, false, false, true, position.x - sprite->getWidth() / 2, position.y - sprite->getHeight() / 2);
	}

}

void MB01GolemShell::normalMode() {
	setPosition(Vector2f(position.x + speed * lookAt.x, position.y + speed * lookAt.y));
	if (explodeTimer.checkTimer() || isThereCollision() || (explodeWhenCollisionWithPlayer && engine->checkCollision(&bBox, player->getBoundingBox()))) {
		mode = MODE_EXPLOSION;
		this->shellExplosionAnimation.resetAnimation();
		explosionTimer.startTimer();
	}
}

void MB01GolemShell::explosionMode() {
	if (explosionTimer.checkTimer()) {
		mode = MODE_INACTIVE;
	}
	//MAKE IT SQUARED LATER
	if (position.getDistance(player->getMidPosition()) <= explosionRadius) {
		player->hurtPlayer();
	}
}

bool MB01GolemShell::isActive() {
	return mode != MODE_INACTIVE;
}

int MB01GolemShell::getExplosionRadius() {
	return explosionRadius;
}