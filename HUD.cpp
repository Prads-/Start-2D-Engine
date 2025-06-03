#include "HUD.h"

HUD::HUD(StartEngine *engine, const FrameCountTimer *specialAttackCooldownTimer, const FrameCountTimer *secondaryMoveCooldownTimer,
		const bool *specialAttackCooldown, const bool *secondaryMoveCooldown, const bool *hasBall, const PickupMove *specialAttack, 
		const PickupMove *secondaryMove, const PickupMove *pickupMove, const Image *pickupSprites, const Image *pickupCross) {
	this->engine = engine;
	this->specialAttackCooldownTimer = specialAttackCooldownTimer;
	this->secondaryMoveCooldownTimer = secondaryMoveCooldownTimer;
	this->specialAttackCooldown = specialAttackCooldown;
	this->secondaryMoveCooldown = secondaryMoveCooldown;
	this->specialAttack = specialAttack;
	this->secondaryMove = secondaryMove;
	this->pickupMove = pickupMove;
	this->pickupSprites = pickupSprites;
	this->hasBall = hasBall;
	this->pickupCross = pickupCross;
	drawHealthBar = true;
}

void HUD::makeHealthBarFull() {
	healthBar.makeHealthBarFull();
}

void HUD::updateHealthBar(int health, int maxHealth) {
	healthBar.updateHealthBar(health, maxHealth);
}

const Image *HUD::getHealthBar() {
	return healthBar.getHealthBar();
}

void HUD::renderHUD() {
	engine->clearHUDBuffer(0);
	if (drawHealthBar) {
		engine->drawHUD(healthBar.getHealthBar(), 0, false, false, false, 0, 0);
	}
	if (*hasBall) {
		engine->drawHUD(&pickupSprites[*secondaryMove], 0, false, false, false, 27, 6);
		engine->drawHUD(&pickupSprites[*specialAttack], 0, false, false, false, 17, 16);
		if (*specialAttackCooldown) {
			int offset = ((float)specialAttackCooldownTimer->getPassedFrames() / specialAttackCooldownTimer->getInterval()) * 30.0f;
			engine->makeRectAreaDarkerHUD(Vector2i(17, 16 + offset), Vector2i(47, 46), 3);
		}
	} else {
		engine->drawHUD(&pickupSprites[*specialAttack], 0, false, false, false, 27, 6);
		engine->drawHUD(&pickupSprites[*secondaryMove], 0, false, false, false, 17, 16);
		if (*secondaryMoveCooldown) {
			int offset = ((float)secondaryMoveCooldownTimer->getPassedFrames() / secondaryMoveCooldownTimer->getInterval()) * 30.0f;
			engine->makeRectAreaDarkerHUD(Vector2i(17, 16 + offset), Vector2i(47, 46), 3);
		}
	}
	if (*pickupMove != PICKUP_NONE) {
		engine->drawHUD(&pickupSprites[*pickupMove], 0, false, false, false, 17, 56);
		if (!checkIfPickupItemCanBeUsed()) {
			engine->drawHUD(pickupCross, 0, false, false, false, 17, 56);
		}
	}
}

void HUD::setDrawHealthBar(bool drawHealthBar) {
	this->drawHealthBar = drawHealthBar;
}

bool HUD::checkIfPickupItemCanBeUsed() {
	if (*hasBall) {
		switch (*pickupMove) {
			case PICKUP_GRANADE:
			case PICKUP_HOMING:
			case PICKUP_INVISIBLE_BALL:
			case PICKUP_PIERCING_BALL:
			case PICKUP_PINBALL:
				return true;
		}
		return false;
	} else {
		switch (*pickupMove) {
			case PICKUP_BATTERY:
			case PICKUP_CALL_BALL:
			case PICKUP_IMMOBILISER:
			case PICKUP_SHIELD:
			case PICKUP_TELEPORT_BALL:
				return true;
		}
		return false;
	}
}