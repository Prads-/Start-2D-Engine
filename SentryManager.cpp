#include "SentryManager.h"
#include "EnemyScore.h"
using namespace std;

SentryManager::SentryManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) 
				: EnemyManager(engine, bBoxes, immobilisers) {
	sentryProjectileSprite.loadImage("Images/enemies/sentry/sentryProjectile.png");
	spriteCannon.loadImage("Images/enemies/sentry/CannonBaseSprite.png");
	spriteDeath.loadImage("Images/enemies/sentry/DeathSprite.png");
	loadSpriteListFile("Images/enemies/sentry/sprites.dat");
	createShadowSprite();
	killScore = ENEMY_SCORE_SENTRY;
}

void SentryManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	Sentry::SpritePack spritePack(&sentryProjectileSprite, &spriteCannon, &spriteDeath);
	Sentry *sentry = new Sentry(engine, &shadowSprite, shadowPeriod, bBoxes, position, player, ball, immobilisers, &sprites, &spritePack);
	if (randomPosition) {
		sentry->setAtRandomPosition();
	}
	enemies.push_back(sentry);
	enemiesCollection.push_back(sentry);
}