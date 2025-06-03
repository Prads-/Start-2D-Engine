#include "PickupItem.h"
#include "MT_RandomGenerator.h"
#include "SfxPool.h"

PickupItem *PickupItem::pickupItem = 0;
Image PickupItem::sprite("Images/pickup.png");

PickupItem::PickupItem(StartEngine *engine, const std::vector<BoundingBox> *bBoxes) : InteractiveObject(engine, bBoxes), inactiveTimer(300) {
	isActive = true;
	width = sprite.getWidth();
	height = sprite.getHeight();
	setAtRandomPosition();
	calculateBB();
}

PickupItem *PickupItem::getInstance(StartEngine *engine, const std::vector<BoundingBox> *bBoxes) {
	cleanUpInstance();
	pickupItem = new PickupItem(engine, bBoxes);
	return pickupItem;
}

PickupItem *PickupItem::getInstance() {
	return pickupItem;
}

void PickupItem::cleanUpInstance() {
	delete pickupItem;
	pickupItem = 0;
}

const Image *PickupItem::getSprite() {
	return &sprite;
}

PickupMove PickupItem::getPickupMove() {
	int randomMove = MT_RandomGenerator::randomNumber() % PICKUP_MAX_VALUE;
	return (PickupMove)randomMove;
}

bool PickupItem::getIsActive() {
	return isActive;
}

void PickupItem::update() {
	if (inactiveTimer.checkTimer()) {
		setAtRandomPosition();
		calculateBB();
		isActive = true;
		engine->playAudioBuffer(SfxPool::getSfxPickupSpawn());
	}
}

void PickupItem::setInactive() {
	isActive = false;
	inactiveTimer.startTimer();
}

void PickupItem::draw() {
	engine->draw(&sprite, 0, false, false, false, position.x, position.y);
}

void PickupItem::pause() {
	inactiveTimer.pauseTimer();
}

void PickupItem::resume() {
	inactiveTimer.resumeTimer();
}