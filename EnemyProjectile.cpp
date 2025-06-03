#include "EnemyProjectile.h"

EnemyProjectile::EnemyProjectile(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, CampaignPlayer *player)
				: InteractiveObject(engine, bBoxes) {
	width = 10;
	height = 10;
	this->player = player;
}

void EnemyProjectile::checkCollisionWithPlayer() {
	if (engine->checkCollision(&bBox, player->getBoundingBox())) {
		player->hurtPlayer();
	}
}