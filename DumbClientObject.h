#ifndef DUMB_CLIENT_OBJECT_H
#define DUMB_CLIENT_OBJECT_H

#include "DumbObject.h"
#include "Image.h"
#include "Animation.h"
#include "HUD.h"
#include "Immobiliser.h"
#include <list>

class DumbClientObject : public DumbObject {
private:
	Image sprite, spriteMoving, spriteBall;
	Animation damageAnimation;
	DataPacketPositions::OpponentPositionData positionPacket;
	DataPacketPositions::MiscPosition miscPosition;
	Vector2i previousPosition;
	FrameCountTimer secondTimer;
	int ballHoldSecondsLeft;
	Image *font, *cursor, *shootFrame;
	int health;
	bool hasBall, specialAttackCooldown, secondaryMoveCooldown, wasDamagedInPreviousFrame,
		wasStunnedInPreviousFrame, wasShieldedInPreviousFrame;
	FrameCountTimer specialAttackCooldownTimer, secondaryMoveCooldownTimer;
	PickupMove specialAttack, secondaryMove, pickupMove;
	Image *pickupSprites, *pickupCross;
	volatile bool newPositionPacket;
	bool hudNeedsUpdate;
	HUD hud;
	std::list<Immobiliser> *immobilisers;
	bool dead, inDeathPeriod;
	FrameCountTimer deathPeriodTimer;
	int width, height;
	Animation stunEffectAnimation, shieldEffectAnimation;

	void drawTargetLine();
	void drawCursor();
	void updateHUD();
	void processMyPositionPacket();
	void processCommonPositionPacket();
	void drawStunnedAndShieldPeriod();

public:
	DumbClientObject(StartEngine *engine, int characterId, Image *font, Image *cursor, Image *shootFrame, Image *pickupSprites, Image *pickupCross, std::list<Immobiliser> *immobilisers);

	void setPositionPacket(DataPacketPositions::OpponentPositionData positionPacket, DataPacketPositions::MiscPosition miscPosition);

	void update();
	void draw();

	bool isDead();
	bool isMe();

	Vector2i getMidPosition();
};

#endif