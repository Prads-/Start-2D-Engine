#ifndef AI_OBJECT_H
#define AI_OBJECT_H

#include "declarations.h"
#include <list>
#include "Competitor.h"
#include "BallObject.h"
#include "PlayerObject.h"
#include "StartEngine.h"
#include "Grid.h"
#include "CharacterVariables.h"

class AIObject : public Competitor {
private:
	enum AIObjectMoveState {
		AI_MOVE_STAND,
		AI_MOVE_RUN,
	}moveState;

	enum AIObjectPlayState {
		AI_GET_BALL,
		AI_RUN_FROM_OPPONENT,
		AI_DODGE_BALL,
		AI_TARGET_OPPONENT,
		AI_SHOOT,
		AI_DO_NOTHING
	}playState;

	enum AIObjectLookSide {
		AI_LEFT,
		AI_RIGHT,
		AI_UP,
		AI_DOWN
	};

	Grid **gridMap;
	std::list<Vector2i> pathPoints;
	Vector2f targetPosition;
	std::vector<const Competitor*> opponents;
	float cautionDistance, shootingSharpness;
	FrameCountTimer difficultyTimer;
	bool difficultySlowdown, hasSeenBall;
	
	void decidePlayStatus();
	void decideRotation();

	void checkPath(Vector2f target);

	const Competitor *getClosestOpponent(Vector2f closestObjPos);

	void targetOpponent();
	void targetOpponentDirectly(Vector2f opponentPosition, bool hasObstacle);

	void decidePickupOrBall();
	void chaseAfterBall();
	void takeAwayPickupOrSecondary(PickupMove pickupMove);
	
	void immobiliseThrower(const Competitor *opponent);
	void runFromObject(const InteractiveObject *obj, bool doRayTest = false);
	void dodgeBall();
	void runFromOpponent();
	void runFromOpponent(const Competitor *opponent);

	BallType chooseIndirectShootingMove();
	BallType chooseDirectShootingMove(bool hasObstacle);
	BallType chooseDirectShootingMoveFromPickupMove(bool hasObstacle);
	BallType chooseDirectShootingMoveFromSpecialThrow(bool hasObstacle);
	void shootDirectly(Vector2f opponentPosition, bool hasObstacle);
	void shootIndirectly(Vector2f opponentPosition, AIObjectLookSide lookSide, bool hasObstacle);
	void shoot(BallType ballType);
	void move();
	
	float getBounceCoordinateValueX(Vector2f opponentPosition, float c);
	float getBounceCoordinateValueX(Vector2f opponentPosition);
	float getBounceCoordinateValueY(Vector2f opponentPosition, float c);
	float getBounceCoordinateValueY(Vector2f opponentPosition);

	void checkPreviousPathValidity(Vector2f *currentTargetPosition);
	void invalidatePath();

	bool hasSecondaryMove(PickupMove pickupMove);

	void rotateMoveVector(Vector2f towards);
	void rotateLookVector(Vector2f towards);
	void initiateDifficultySlowdown();
	void makeTargetPositionInvalid();

public:
	AIObject(StartEngine *engine, BallObject *ball, const std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers, 
		AIVariables variables, std::string characterPath);
	~AIObject();

	void setOpponents(std::vector<const Competitor*> *opponents);
	void update();
	void draw();
	bool isMoving() const;

	void setAIDifficulty(AIDifficulty difficulty);

	void pause();
	void resume();
};

#endif