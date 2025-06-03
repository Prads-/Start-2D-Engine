#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include "declarations.h"
#include "InteractiveObject.h"
#include "StartEngine.h"
#include <vector>
#include "PickupEnums.h"
#include "FrameCountTimer.h"
#include "Grid.h"
#include <list>

#define BALL_WIDTH		20
#define BALL_HEIGHT		20

class Enemy;
class Competitor;

class BallObject : public InteractiveObject {
private:
	Image sprite, spriteBallHit, spriteBallIdle, spriteCallMark, spriteHitEffect;
	bool isTaken, isInvisible;
	std::vector<const Enemy*> enemies;
	Competitor *shooter, *caller;
	BallType ballType;
	FrameCountTimer hitTimer;
	bool hitFlag, reverseLock, hasExploded;
	float callDistanceTravelled;
	Vector2f callPoint;
	Grid **gridMap;
	std::list<Vector2i> pathPoints;
	float radius, idleVisibilityCircle;
	uint32 score;

	void decelerate();
	void checkCollision();
	void collisionWithObject(const BoundingBox *bBox);

	void moveNormal();
	void moveGranade();
	void moveHoming();

	void explode();
	const Enemy *getClosestEnemy(float *distanceOut = 0);

	void doStopCheck();
	void moveTowardsCallPoint();
	void drawIdleVisibilityCircle();

public:
	BallObject(StartEngine *engine, const std::vector<BoundingBox> *bBoxes);
	~BallObject();

	void setEnemies(std::vector<const Enemy*> enemies);

	void update();
	
	void setAtShootSpeed();
	void setLook(Vector2f lookAt);

	bool taken();
	void setTaken(bool isTaken);

	void draw();

	void setAtRandomPosition();
	void setShooter(Competitor *shooter);
	Competitor *getShooter();

	BallType getBallType();
	void setBallType(BallType type);

	bool getIsInvisible();
	void setIsInvisible(bool isInvisible);

	bool getHitFlag();
	void setHitTimer();

	bool getHasExploded();

	void clearEnemies();

	bool callBall(Vector2f callPoint, Competitor *caller);
	Competitor *getCaller();

	float getRadius();
	void reverseLookAt();

	void addToScore(uint32 value);
	uint32 getScore();

	void updateGridMap();
};

#endif