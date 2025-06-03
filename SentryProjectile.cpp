#include "SentryProjectile.h"

SentryProjectile::SentryProjectile(StartEngine *engine, Image *sprite, const std::vector<BoundingBox> *bBoxes, CampaignPlayer *player, float speed) 
				: EnemyProjectile(engine, bBoxes, player) {
	width = 10;
	height = 10;
	this->speed = speed;
	this->sprite = sprite;
}

void SentryProjectile::update() {
	setPosition(Vector2f(position.x + lookAt.x * speed, position.y + lookAt.y * speed));
	checkCollisionWithPlayer();
}

void SentryProjectile::draw() {
	engine->draw(sprite, rotateDegree, false, false, false, position.x, position.y);
}