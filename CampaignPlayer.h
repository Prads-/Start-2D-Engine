#ifndef CAMPAIGN_PLAYER_H
#define CAMPAIGN_PLAYER_H

#include "PlayerObject.h"
#include <vector>
#include "CampaignPickupItem.h"

class CampaignPlayer : public PlayerObject {
private:
	uint32 pickupUsageCount;
	bool immuneFromEnemies;
	FrameCountTimer immuneFromEnemiesTimer;
	std::vector<CampaignPickupItem> *pickupItems;
	Image pickupCount;

	void checkCollisionWithPickup();
	bool checkBallHoldTimer();
	void takeAwayPickupMove();
	virtual void renderHUD();

public:
	CampaignPlayer(StartEngine *engine, BallObject *ball, Image *font, const std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers,
		std::string characterPath, PlayerVariables playerVariables, std::vector<CampaignPickupItem> *pickupItems, Image *pickupSprites,
		const Image *pickupCross, int health);

	void hurtPlayer(int hp = 1);
	void update();

	void draw();
	void setDrawHealthBar(bool drawHealthBar);

	void clearPickupMove();
};

#endif