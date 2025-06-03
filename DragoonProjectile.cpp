#include "DragoonProjectile.h"

DragoonProjectile::DragoonProjectile(StartEngine *engine, Image *sprite, const std::vector<BoundingBox> *bBoxes, CampaignPlayer *player, Vector2f position, Vector2f lookAt, float speed) 
					: EnemyProjectile(engine, bBoxes, player) {
	width = 10;
	height = 10;
	this->sprite = sprite;
	this->position = position;
	this->lookAt = lookAt;
	this->speed = speed;
	startPosition = position;
}

void DragoonProjectile::update() {
	setPosition(Vector2f(position.x + lookAt.x * speed, position.y += lookAt.y * speed));
	checkCollisionWithPlayer();
}

void DragoonProjectile::draw() {
	engine->draw(sprite, 0, false, false, false, position.x, position.y);
}

Vector2f DragoonProjectile::getStartPosition() {
	return startPosition;
}