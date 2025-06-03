#ifndef SENTRY_MANAGER_H
#define SENTRY_MANAGER_H

#include "EnemyManager.h"
#include <list>
#include "Sentry.h"
#include "Image.h"
#include "CampaignPlayer.h"
#include "BallObject.h"
#include "StartEngine.h"

class SentryManager : public EnemyManager {
private:
	Image sentryProjectileSprite, spriteCannon, spriteDeath;

public:
	SentryManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);
	
	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif