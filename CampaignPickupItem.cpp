#include "CampaignPickupItem.h"
#include "MT_RandomGenerator.h"
#include "SfxPool.h"

CampaignPickupItem::CampaignPickupItem(StartEngine *engine, Vector2f position, std::vector<PickupMove> acquirableMoves, int timerInterval,  
									   PickupItemSprites pickupSprites, int usageCount, int respawnCount, std::vector<BoundingBox> *bBoxes)
					: InteractiveObject(engine, bBoxes), inactiveTimer(timerInterval) {
	width = pickupSprites.pickupSprites[0].getWidth();
	height = pickupSprites.pickupSprites[0].getHeight();
	this->acquirableMoves = acquirableMoves;
	this->usageCount = usageCount;
	isActive = acquirableMoves.size() > 0;
	if (timerInterval == 0) {
		this->respawnCount = 1;
	} else {
		if (respawnCount == 0) {
			this->respawnCount = 99999;
		} else {
			this->respawnCount = respawnCount;
		}
	}
	setPosition(position);
	if (isActive) {
		getRandomMove();
	}

	this->pickupSprites = pickupSprites.pickupSprites;
	spriteNoRespawnLeft = pickupSprites.spriteNoRespawnLeft;
}

PickupMove CampaignPickupItem::getPickupMove() {
	return currentPickupMove;	
}

bool CampaignPickupItem::getIsActive() {
	return isActive;
}

void CampaignPickupItem::update() {
	if (!isActive && respawnCount > 0) {
		if (inactiveTimer.checkTimer()) {
			getRandomMove();
			isActive = true;
			engine->playAudioBuffer(SfxPool::getSfxPickupSpawn());
		}
	}
}

void CampaignPickupItem::setInactive() {
	--respawnCount;
	isActive = false;
	inactiveTimer.startTimer();
}

void CampaignPickupItem::draw() {
	if (isActive || respawnCount <= 0) {
		engine->draw(getPickupMoveSprite(), 0, false, false, false, position.x, position.y);
	}
}

void CampaignPickupItem::pause() {
	inactiveTimer.pauseTimer();
}

void CampaignPickupItem::resume() {
	inactiveTimer.resumeTimer();
}

Image *CampaignPickupItem::getPickupMoveSprite() {
	if (respawnCount <= 0) {
		return spriteNoRespawnLeft;
	} else {
		return &pickupSprites[currentPickupMove];
	}
}

void CampaignPickupItem::getRandomMove() {
	currentPickupMove = acquirableMoves[MT_RandomGenerator::randomNumber() % acquirableMoves.size()];
}

uint32 CampaignPickupItem::getUsageCount() {
	return usageCount;
}