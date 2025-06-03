#include "PatrollerProjectile.h"

PatrollerProjectile::PatrollerProjectile(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, CampaignPlayer *player, Vector2f position, Vector2f lookAt, float speed, Image *sprite) 
					: EnemyProjectile(engine, bBoxes, player) {
	width = 10;
	height = 10;
	setPosition(position);
	this->lookAt = lookAt;
	this->speed = speed;
	this->sprite = sprite;
}

void PatrollerProjectile::update() {
	setPosition(Vector2f(position.x + lookAt.x * speed, position.y + lookAt.y * speed));
	checkCollisionWithPlayer();
}

void PatrollerProjectile::draw() {
	engine->draw(sprite, 0, false, false, false, position.x, position.y);
}