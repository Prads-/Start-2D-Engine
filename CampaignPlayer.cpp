#include "CampaignPlayer.h"
#include "stringExtension.h"
using namespace StringExtension;

#define PICKUP_COUNT_WIDTH		20
#define PICKUP_COUNT_HEIGHT		20

CampaignPlayer::CampaignPlayer(StartEngine *engine, BallObject *ball, Image *font, const std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers,
		std::string characterPath, PlayerVariables playerVariables, std::vector<CampaignPickupItem> *pickupItems, Image *pickupSprites, 
		const Image *pickupCross, int health) 
		: PlayerObject(engine, ball, font, bBoxes, immobilisers, characterPath, playerVariables, pickupSprites, pickupCross, health), 
		immuneFromEnemiesTimer(60) {
	immuneFromEnemies = 0;
	this->pickupItems = pickupItems;
	pickupUsageCount = 0;
	pickupCount.createImage(PICKUP_COUNT_WIDTH, PICKUP_COUNT_HEIGHT);
	uint32 *pickupCountImage = pickupCount.getModifiablePtr();
	for (int i = 0; i < PICKUP_COUNT_WIDTH * PICKUP_COUNT_HEIGHT; ++i) {
		pickupCountImage[i] = 0x7F010101;
	}
}

void CampaignPlayer::hurtPlayer(int hp) {
	if (!immuneFromEnemies && !shieldActive) {
		takeAwayHP(hp);
		immuneFromEnemies = true;
		immuneFromEnemiesTimer.startTimer();
	}
}

void CampaignPlayer::update() {
	PlayerObject::update();
	if (immuneFromEnemies) {
		immuneFromEnemies = !immuneFromEnemiesTimer.checkTimer();
	}
}

void CampaignPlayer::checkCollisionWithPickup() {
	for (CampaignPickupItem &pickupItem : *pickupItems) {
		if (pickupItem.getIsActive() && engine->checkCollision(&bBox, pickupItem.getBoundingBox())) {
			PickupMove newMove = pickupItem.getPickupMove();
			if (pickupMove == newMove) {
				pickupUsageCount += pickupItem.getUsageCount();
			} else {
				pickupUsageCount = pickupItem.getUsageCount();
				pickupMove = newMove;
			}
			if (pickupUsageCount > 9) {
				pickupUsageCount = 9;
			}
			pickupItem.setInactive();
			renderHUD();
			break;
		}
	}
}

void CampaignPlayer::draw() {
	if (isInDeathPeriod()) {
		drawDeathPeriod(&sprite);
		return;
	}

	calculateRotateFromLook();
	drawTargetLine();

	if (_hasBall) {
		engine->draw(&spriteBall, rotateDegree, false, false, false, position.x, position.y);
	}

	if (playDamageAnimation) {
		const Image *frame;
		if (damageAnimation.getAnimationFrame(&frame)) {
			playDamageAnimation = false;
		}
		engine->draw(frame, rotateDegree, false, false, false, position.x, position.y);
	} else {
		if (moveState == PLAYER_MOVE_RUN) {
			engine->draw(&spriteMoving, rotateDegree, false, false, false, position.x, position.y);
		} else {
			engine->draw(&sprite, rotateDegree, false, false, false, position.x, position.y);
		}
	}

	drawStunEffect();
	drawShieldEffect();
}

bool CampaignPlayer::checkBallHoldTimer() {
	return false;
}

void CampaignPlayer::takeAwayPickupMove() {
	--pickupUsageCount;
	if (pickupUsageCount < 1) {
		pickupMove = PICKUP_NONE;
	}
	renderHUD();
}

void CampaignPlayer::renderHUD() {
	PlayerObject::renderHUD();
	if (pickupMove != PICKUP_NONE) {
		engine->drawHUD(&pickupCount, 0, false, false, true, 37, 76);
		engine->drawTextHUD(intToStr(pickupUsageCount), font, 32, 32, 16, 0xFFFFFF00, 40, 76);
	}
}

void CampaignPlayer::setDrawHealthBar(bool drawHealthBar) {
	hud.setDrawHealthBar(drawHealthBar);
}

void CampaignPlayer::clearPickupMove() {
	Competitor::clearPickupMove();
	pickupUsageCount = 0;
}