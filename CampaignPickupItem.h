#ifndef CAMPAIGN_PICKUP_ITEM_H
#define CAMPAIGN_PICKUP_ITEM_H

#include "InteractiveObject.h"
#include "Image.h"
#include "FrameCountTimer.h"
#include <vector>
#include "PickupEnums.h"

class CampaignPickupItem : public InteractiveObject {
private:
	Image *pickupSprites, *spriteNoRespawnLeft;
	bool isActive;
	FrameCountTimer inactiveTimer;
	std::vector<PickupMove> acquirableMoves;
	PickupMove currentPickupMove;
	int usageCount, respawnCount;

	Image *getPickupMoveSprite();
	void getRandomMove();

public:
	struct PickupItemSprites {
		Image *pickupSprites, *spriteNoRespawnLeft;

		PickupItemSprites(Image *pickupSprites, Image *spriteNoRespawnLeft) {
			this->pickupSprites = pickupSprites;
			this->spriteNoRespawnLeft = spriteNoRespawnLeft;
		}
	};

	CampaignPickupItem(StartEngine *engine, Vector2f position, std::vector<PickupMove> acquirableMoves, int timerInterval, 
		PickupItemSprites pickupSprites, int usageCount, int respawnCount, std::vector<BoundingBox> *bBoxes = 0);

	PickupMove getPickupMove();
	bool getIsActive();
	uint32 getUsageCount();

	void update();
	void setInactive();

	void draw();

	void pause();
	void resume();
};

#endif