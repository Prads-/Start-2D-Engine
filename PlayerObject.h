#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "declarations.h"
#include "Image.h"
#include "Competitor.h"
#include "StartEngine.h"
#include "BallObject.h"
#include "VirtualCursor.h"
#include "CharacterVariables.h"
#include "HUD.h"

#define PLAYER_RADIUS_SQUARED			400.0f

class PlayerObject : public Competitor {
protected:
	enum PlayerMoveState {
		PLAYER_MOVE_STAND,
		PLAYER_MOVE_RUN
	}moveState;

	Image *font;
	int ballHoldSecondsLeft;
	FrameCountTimer ballHoldSecondsTimer;
	int maxHealth;
	const Image *pickupSprites;
	bool hudWasUpdated, makeCursorMovementRelative;
	VirtualCursor virtualCursor;
	HUD hud;

	void move();
	void rotate();

	void checkMoveState();

	void collisionWithBall();
	void collisionWithObjects(const BoundingBox *bBox);

	void checkBallThrow();
	bool checkSpecialThrow(PickupMove specialThrow);
	bool checkSecondaryAbility(PickupMove secondaryAbility);
	bool checkBallHoldTimer();
	void shoot(PickupMove move);
	
	virtual void drawTargetLine();
	virtual void makeHealthBarFull();
	virtual void updateHealthBar();
	virtual void takeAwayHP(int hp = 1);
	void updateCursorPosition();
	virtual void updateVirtualCursor();

	virtual void takeAwayPickupMove();

	virtual bool isKeyPressed(int key);
	virtual bool wasMouseLeftToggled();
	virtual bool wasMouseMidToggled();
	virtual bool wasMouseRightToggled();
	
	void playBallShootSfx();
	void playHurtSfx();
	void playDeathSfx();
	virtual void checkCursorMovementToggle();

public:
	PlayerObject(StartEngine *engine, BallObject *ball, Image *font, const std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers,
		std::string characterPath, PlayerVariables playerVariables, Image *pickupSprites, const Image *pickupCross, int health);

	virtual void update();
	virtual void draw();
	virtual void presentHUD();
	virtual void renderHUD();

	bool isMoving() const;
	
	void pause();
	void resume();
	virtual void drawCursor();
	void setAIDifficulty(AIDifficulty difficulty);
};

#endif