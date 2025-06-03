#include "PatrolRover.h"
#include "MT_RandomGenerator.h"

PatrolRover::PatrolRover(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
			int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, Vector2f position)
			: Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites), cooldownTimer(60), directionChangeTimer(90) {
	width = 40;
	height = 40;
	radius = 40.0f / 2.0f;
	commonInit(position, player);
	frontSensorLength = 20.0f;
	calculateSensorBB();
}

PatrolRover::PatrolRover(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, 
		Vector2f position, int width, int height, int frontSensorLength)
		: Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites), cooldownTimer(60), directionChangeTimer(90){
	this->width = width;
	this->height = height;
	radius = width / 2.0f;
	commonInit(position, player);
	this->frontSensorLength = frontSensorLength;
	calculateSensorBB();
}

void PatrolRover::commonInit(Vector2f position, CampaignPlayer *player) {
	setPosition(position);
	isCoolingDown = false;
	changeDirection = false;
	currentRotateDegrees = MT_RandomGenerator::randomNumber() % 360;
	calculateFacingRotateDegrees();
	currentRotateDegrees = rotateDegree;
	calculateLook();
	this->player = player;
	sideSensorLength = 120.0f;
}

void PatrolRover::move() {
	float xSpeed = lookAt.x * speed, ySpeed = lookAt.y * speed;
	
	setPosition(Vector2f(position.x + xSpeed, position.y + ySpeed));
	
	sensorBBFront.minP = Vector2f(sensorBBFront.minP.x + xSpeed, sensorBBFront.minP.y + ySpeed);
	sensorBBFront.maxP = Vector2f(sensorBBFront.maxP.x + xSpeed, sensorBBFront.maxP.y + ySpeed);

	sensorBBLeft.minP = Vector2f(sensorBBLeft.minP.x + xSpeed, sensorBBLeft.minP.y + ySpeed);
	sensorBBLeft.maxP = Vector2f(sensorBBLeft.maxP.x + xSpeed, sensorBBLeft.maxP.y + ySpeed);

	sensorBBRight.minP = Vector2f(sensorBBRight.minP.x + xSpeed, sensorBBRight.minP.y + ySpeed);
	sensorBBRight.maxP = Vector2f(sensorBBRight.maxP.x + xSpeed, sensorBBRight.maxP.y + ySpeed);
}

void PatrolRover::checkChangeDirection() {
	if (changeDirection) {
		if (decideToRotate()) {
			changeDirection = false;
			directionChangeTimer.startTimer();
		} else {
			checkFrontSensor();
		}
	} else {
		checkFrontSensor();
		changeDirection = directionChangeTimer.checkTimer();
	}
}

void PatrolRover::checkFrontSensor() {
	if (checkCollisionWithSensor(&sensorBBFront)) {
		if (checkSideSensors()) {
			changeDirection = false;
			directionChangeTimer.startTimer();
		} else {
			reverse();
		}
	}
}

void PatrolRover::calculateSensorBB() {
	const float SENSOR_BB_FRONT_SIZE = frontSensorLength, SENSOR_BB_SIZE = sideSensorLength, SENSOR_BB_EXCESS = 3.0f;

	if (lookAt.x == 0.0f) {
		if (lookAt.y == 1.0f) { //Down
			sensorBBFront = BoundingBox(Vector2f(position.x, bBox.maxP.y), Vector2f(bBox.maxP.x, bBox.maxP.y + SENSOR_BB_FRONT_SIZE));
			sensorBBLeft = BoundingBox(Vector2f(bBox.maxP.x, position.y - SENSOR_BB_EXCESS), Vector2f(bBox.maxP.x + SENSOR_BB_SIZE, bBox.maxP.y)); 
			sensorBBRight = BoundingBox(Vector2f(position.x - SENSOR_BB_SIZE, position.y - SENSOR_BB_EXCESS), sensorBBFront.minP);
		} else { //Up
			sensorBBFront = BoundingBox(Vector2f(position.x, position.y - SENSOR_BB_FRONT_SIZE), Vector2f(bBox.maxP.x, position.y));
			sensorBBLeft = BoundingBox(Vector2f(position.x - SENSOR_BB_SIZE, position.y), Vector2f(position.x, bBox.maxP.y + SENSOR_BB_EXCESS));
			sensorBBRight = BoundingBox(sensorBBFront.maxP, Vector2f(bBox.maxP.x + SENSOR_BB_SIZE, bBox.maxP.y + SENSOR_BB_EXCESS));
		}
	} else {
		if (lookAt.x == 1.0f) { //Right
			sensorBBFront = BoundingBox(Vector2f(bBox.maxP.x, position.y), Vector2f(bBox.maxP.x + SENSOR_BB_FRONT_SIZE, bBox.maxP.y));
			sensorBBLeft = BoundingBox(Vector2f(position.x - SENSOR_BB_EXCESS, position.y - SENSOR_BB_SIZE), sensorBBFront.minP);
			sensorBBRight = BoundingBox(Vector2f(position.x - SENSOR_BB_EXCESS, bBox.maxP.y), Vector2f(bBox.maxP.x, bBox.maxP.y + SENSOR_BB_SIZE));
		} else { //Left
			sensorBBFront = BoundingBox(Vector2f(position.x - SENSOR_BB_FRONT_SIZE, position.y), Vector2f(position.x, bBox.maxP.y));
			sensorBBLeft = BoundingBox(Vector2f(position.x, bBox.maxP.y), Vector2f(bBox.maxP.x + SENSOR_BB_EXCESS, bBox.maxP.y + SENSOR_BB_SIZE));
			sensorBBRight = BoundingBox(Vector2f(position.x, position.y - SENSOR_BB_SIZE), Vector2f(bBox.maxP.x + SENSOR_BB_EXCESS, position.y));
		}
	}
}

bool PatrolRover::checkCollisionWithSensor(const BoundingBox *sensorBB) {
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, sensorBB)) {
			return true;
		}
	}
	return false;
}

void PatrolRover::rotate(bool left) {
	if (left) {
		lookAt = Vector2f(lookAt.y, -lookAt.x);
	} else {
		lookAt = Vector2f(-lookAt.y, lookAt.x);
	}
	calculateRotateDegrees();
	calculateSensorBB();
}

void PatrolRover::calculateRotateDegrees() {
	if (lookAt.x != 0.0f) {
		if (lookAt.x < 0.0f) {
			rotateDegree = 270;
		} else {
			rotateDegree = 90;
		}
	} else {
		if (lookAt.y < 0.0f) {
			rotateDegree = 0.0f;
		} else { 
			rotateDegree = 180;
		}
	}
}

bool PatrolRover::checkCurrentRotation() {
	if (currentRotateDegrees != rotateDegree) {
		if (rotateDegree > currentRotateDegrees) {
			int difference = rotateDegree - currentRotateDegrees;
			if (difference < 180) {
				currentRotateDegrees += (difference < 5) ? difference : 5;
			} else {
				currentRotateDegrees -= (difference < 5) ? difference : 5;
			}
		} else {
			int difference = currentRotateDegrees - rotateDegree;
			if (difference < 180) {
				currentRotateDegrees -= (difference < 5) ? difference : 5;
			} else {
				currentRotateDegrees += (difference < 5) ? difference : 5;
			}
		}
		if (currentRotateDegrees < 0) {
			currentRotateDegrees += 360;
		} else if (currentRotateDegrees > 359) {
			currentRotateDegrees -= 360;
		}
		return false;
	}
	return true;
}

void PatrolRover::calculateFacingRotateDegrees() {
	if (currentRotateDegrees <= 45) {
		rotateDegree = 0;
	} else if (currentRotateDegrees <= 135) {
		rotateDegree = 90;
	} else if (currentRotateDegrees <= 225) {
		rotateDegree = 180;
	} else if (currentRotateDegrees <= 315) {
		rotateDegree = 270;
	} else {
		rotateDegree = 0;
	}
	directionChangeTimer.startTimer();
}

void PatrolRover::calculateLook() {
	switch (rotateDegree) {
		case 0:
			lookAt = Vector2f(0.0f, -1.0f);
			break;
		case 90:
			lookAt = Vector2f(1.0f, 0.0f);
			break;
		case 180:
			lookAt = Vector2f(0.0f, 1.0f);
			break;
		case 270:
			lookAt = Vector2f(-1.0f, 0.0f);
	}
}

void PatrolRover::reverse() {
	lookAt.x *= -1.0f;
	lookAt.y *= -1.0f;
	calculateRotateFromLook();
	calculateSensorBB();
}

bool PatrolRover::checkSideSensors() {
	bool noCollisionWithLeftSensor = !checkCollisionWithSensor(&sensorBBLeft),
		noCollisionWithRightSensor = !checkCollisionWithSensor(&sensorBBRight);
	if (noCollisionWithLeftSensor && noCollisionWithRightSensor) {
		bool takeLeftDirection = MT_RandomGenerator::randomNumber() & 0x8000;
		rotate(takeLeftDirection);
		return true;
	} else if (noCollisionWithLeftSensor) {
		rotate(true);
		return true;
	} else if (noCollisionWithRightSensor) {
		rotate(false);
		return true;
	}
	return false;
}

bool PatrolRover::decideToRotate() {
	bool noCollisionWithLeftSensor = !checkCollisionWithSensor(&sensorBBLeft),
		noCollisionWithRightSensor = !checkCollisionWithSensor(&sensorBBRight);
	if (noCollisionWithLeftSensor && noCollisionWithRightSensor) {
		int directionChangeRandom = MT_RandomGenerator::randomNumber() % 4;
		if (directionChangeRandom == 0) {
			rotate(true);
		} else if (directionChangeRandom == 1) {
			rotate(false);
		}
		return true;
	} else if (noCollisionWithLeftSensor) {
		int directionChangeRandom = MT_RandomGenerator::randomNumber() % 2;
		if (directionChangeRandom == 0) {
			rotate(true);
		}
		return true;
	} else if (noCollisionWithRightSensor) {
		int directionChangeRandom = MT_RandomGenerator::randomNumber() % 2;
		if (directionChangeRandom == 0) {
			rotate(false);
		}
		return true;
	}
	return false;
}

void PatrolRover::pause() {
	Enemy::pause();
	cooldownTimer.pauseTimer();
	directionChangeTimer.pauseTimer();
}

void PatrolRover::resume() {
	Enemy::resume();
	cooldownTimer.resumeTimer();
	directionChangeTimer.resumeTimer();
}