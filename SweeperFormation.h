#ifndef SWEEPER_FORMATION_H
#define SWEEPER_FORMATION_H

#include "vector2.h"
#include "BoundingBox.h"
#include "FrameCountTimer.h"
#include "Sweeper.h"
#include <list>
#include <vector>
#include "CampaignPlayer.h"
#include "Grid.h"

class SweeperFormation {
public:
	enum Direction {
		DIRECTION_UP = 0, DIRECTION_RIGHT = 1, DIRECTION_DOWN = 2, DIRECTION_LEFT = 3
	};

private:
	struct Slot {
		Vector2f position;
	};

	enum Formation {
		FORMATION_ARROW = 0x10, FORMATION_TURTLE = 0x20, FORMATION_COMPACT_TURTLE = 0x30, FORMATION_SERPENT = 0x40
	}formation;

	enum Mode {
		MODE_NONE, MODE_DASH, MODE_TRANSFORMATION, MODE_FINAL_DASH
	}mode;

	StartEngine *engine;
	Slot midSlot, rightSlots[2], leftSlots[2];
	Slot *slots[5];
	Vector2f absolutePostion;
	BoundingBox formationBBoxes[4];
	FrameCountTimer directionTimer, dashCooldownTimer;
	Sweeper *dasher;
	bool transformationIsValid, dashCooldownPeriod, dashPreparation;
	std::list<Sweeper*> sweepers, sweepersCollection, deadSweepers;
	Direction direction;
	Vector2f moveVector;
	const std::vector<BoundingBox> *bBoxes;
	CampaignPlayer *player;
	bool isInInitialShadowPeriod;
	
	void rearrangeSweepers();
	void moveSweepersInFormation();
	void chooseDirection();
	void checkCollision();
	void checkArrowCollision();
	void checkTurtleCollision();
	void checkCompactTurtleCollision();
	void checkSerpentCollision();
	Direction getReverseDirection(Direction currentDirection);
	void reverseDirection();
	bool checkMidSlotCollision();
	void doTransformation();
	void checkRotation();
	BoundingBox getRotationCollisionCheckBB(Direction direction) const;
	Direction getPerpendicularRandomDirection();
	void updateDash();
	void checkInitiateDash();
	void initiateDash();
	void getRandomDasher();
	void drawSweepers();
	void checkSweepersCollisionWithPlayerAndBall();
	void stopDash();
	void updateFormation();
	void updateFinalDash();
	void updateShadowPeriod();
	void updateDeadSweepers();
	void drawDeadSweepers();
	void updateSweepersLookAt();

	void rearrangeFormation(Vector2f mid, Vector2f right1, Vector2f right2, Vector2f left1, Vector2f left2);
	void recalculateFormationBBoxes(float bb1MinX, float bb1MinY, float bb1MaxX, float bb1MaxY,
		float bb2MinX, float bb2MinY, float bb2MaxX, float bb2MaxY,
		float bb3MinX, float bb3MinY, float bb3MaxX, float bb3MaxY,
		float bb4MinX, float bb4MinY, float bb4MaxX, float bb4MaxY);

	void rearrangeArrowUp();
	void rearrangeArrowRight();
	void rearrangeArrowDown();
	void rearrangeArrowLeft();
	void rearrangeTurtleUp();
	void rearrangeTurtleRight();
	void rearrangeTurtleDown();
	void rearrangeTurtleLeft();
	void rearrangeCompactTurtleUp();
	void rearrangeCompactTurtleRight();
	void rearrangeCompactTurtleDown();
	void rearrangeCompactTurtleLeft();
	void rearrangeSerpentUp();
	void rearrangeSerpentRight();
	void rearrangeSerpentDown();
	void rearrangeSerpentLeft();

public:
	SweeperFormation(StartEngine *engine, Vector2f absolutePosition, const std::vector<BoundingBox> *bBoxes,
		Image *shadowSprite, int initialShadowPeriod, CampaignPlayer *player, 
		BallObject *ball, Grid** gridMap, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, Sweeper::SpritePack *spritePack);
	~SweeperFormation();

	void update();
	void draw();

	bool isDead();

	BoundingBox getBoundingBox() const;
	void getEnemiesPointer(std::vector<const Enemy*> *enemiesPtrOut);

	void setInRandomPosition();
	bool isThereCollision(BoundingBox bBox);

	void pause();
	void resume();
};

#endif