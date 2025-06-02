#include "TheEyeProjectile.h"

TheEyeProjectile::TheEyeProjectile(StartEngine *engine, CampaignPlayer *player, Vector2f direction, float speed, Vector2f position, const Image *sprite) 
				: EnemyProjectile(engine, 0, player) {
	isActive = true;
	lookAt = direction;
	this->speed = speed;
	setPosition(position);
	this->sprite = sprite;
}

void TheEyeProjectile::update() {
	setPosition(Vector2f(position.x + lookAt.x * speed, position.y + lookAt.y * speed));
	if (position.x < 0.0f || position.x >= FRAME_WIDTH || position.y < 0.0f || position.y >= FRAME_HEIGHT) {
		isActive = false;
	}
	checkCollisionWithPlayer();
}

void TheEyeProjectile::draw() {
	engine->draw(sprite, 0, false, false, false, position.x, position.y);
}

bool TheEyeProjectile::getIsActive() {
	return isActive;
}