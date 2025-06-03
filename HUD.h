#ifndef HUD_H
#define HUD_H

#include "StartEngine.h"
#include "HealthBar.h"
#include "FrameCountTimer.h"
#include "PickupEnums.h"
#include "Image.h"

class HUD {
private:
	StartEngine *engine;
	HealthBar healthBar;
	const FrameCountTimer *specialAttackCooldownTimer, *secondaryMoveCooldownTimer;
	const bool *specialAttackCooldown, *secondaryMoveCooldown, *hasBall;
	const PickupMove *specialAttack, *secondaryMove, *pickupMove;
	const Image *pickupSprites, *pickupCross;
	bool drawHealthBar;

	bool checkIfPickupItemCanBeUsed();

public:
	HUD(StartEngine *engine, const FrameCountTimer *specialAttackCooldownTimer, const FrameCountTimer *secondaryMoveCooldownTimer,
		const bool *specialAttackCooldown, const bool *secondaryMoveCooldown, const bool *hasBall, const PickupMove *specialAttack, 
		const PickupMove *secondaryMove, const PickupMove *pickupMove, const Image *pickupSprites, const Image *pickupCross);

	void makeHealthBarFull();
	void updateHealthBar(int health, int maxHealth);
	const Image *getHealthBar();

	void renderHUD();
	void setDrawHealthBar(bool drawHealthBar);
};

#endif