#ifndef PATROL_ROVER_H
#define PATROL_ROVER_H

#include "Enemy.h"
#include "BoundingBox.h"
#include "CampaignPlayer.h"

class PatrolRover : public Enemy {
protected:
	BoundingBox sensorBBFront, sensorBBLeft, sensorBBRight;
	FrameCountTimer cooldownTimer, directionChangeTimer;
	bool isCoolingDown, changeDirection;
	CampaignPlayer *player;
	int currentRotateDegrees;

	float sideSensorLength, frontSensorLength;

	void move();
	void checkChangeDirection();
	void checkFrontSensor();
	bool checkCollisionWithSensor(const BoundingBox *sensorBB);
	void rotate(bool left);
	void reverse();
	bool checkSideSensors();
	bool decideToRotate();
	void calculateRotateDegrees();
	virtual bool checkCurrentRotation();
	void calculateFacingRotateDegrees();
	void calculateLook();
	void commonInit(Vector2f position, CampaignPlayer *player);

public:
	PatrolRover(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, Vector2f position);
	PatrolRover(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, 
		Vector2f position, int width, int height, int frontSensorLength);

	void calculateSensorBB();

	virtual void pause();
	virtual void resume();
};

#endif