#ifndef PICKUP_ITEM_H
#define PICKUP_ITEM_H

#include "BoundingBox.h"
#include "Image.h"
#include "InteractiveObject.h"
#include "FrameCountTimer.h"
#include "StartEngine.h"
#include "PickupEnums.h"

class PickupItem : public InteractiveObject {
private:
	static Image sprite;
	bool isActive;
	FrameCountTimer inactiveTimer;
	static PickupItem *pickupItem;
	PickupItem(StartEngine *engine, const std::vector<BoundingBox> *bBoxes);

public:
	static PickupItem *getInstance(StartEngine *engine, const std::vector<BoundingBox> *bBoxes);
	static PickupItem *getInstance();
	static void cleanUpInstance();
	static const Image *getSprite();

	PickupMove getPickupMove();
	bool getIsActive();

	void update();
	void setInactive();

	void draw();

	void pause();
	void resume();
};

#endif