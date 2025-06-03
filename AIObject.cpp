#include "AIObject.h"
#include "DynamicMultidimensionalArray.h"
#include "RayIntersection.h"
#include <cmath>
#include "SfxPool.h"
using namespace std;
using namespace DynamicMultidimensionalArray;
using namespace RayIntersection;

#define AI_NORMAL_ROTATION_PER_FRAME	4

AIObject::AIObject(StartEngine *engine, BallObject *ball, const vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers, 
		AIVariables variables, std::string characterPath) : 
		Competitor(engine, ball, immobilisers, bBoxes, characterPath, (PickupMove)variables.specialThrow, (PickupMove)variables.secondaryAbility, COMPETITOR_START_HEALTH), difficultyTimer(0) {
	this->opponents = opponents;
	width = variables.width;
	height = variables.height;
	radius = width / 2;
	_hasBall = false;
	engine->createGridMap(&gridMap, bBoxes, width, height);
	speed = variables.maxSpeed;
	difficultySlowdown = false;
	
	maxSpeed = variables.maxSpeed;
	cautionDistance = variables.cautionDistance;
	shootingSharpness = variables.shootingSharpness;
	makeTargetPositionInvalid();
}

AIObject::~AIObject() {
	engine->destroyGridMap(gridMap, width);
}

void AIObject::setOpponents(std::vector<const Competitor*> *opponents) {
	this->opponents = *opponents;
}

void AIObject::decidePlayStatus() {
	if (_hasBall) {
		if (playState != AI_TARGET_OPPONENT) {
			playState = AI_TARGET_OPPONENT;
			initiateDifficultySlowdown();
			makeTargetPositionInvalid();
		}
	} else if (ball->taken()) {
		if (playState != AI_RUN_FROM_OPPONENT) {
			playState = AI_RUN_FROM_OPPONENT;
			pathPoints.clear();
			initiateDifficultySlowdown();
			makeTargetPositionInvalid();
		}
	} else {
		if (isImmune || ball->getSpeed() <= 3.0f || ball->getPosition().getDistanceSquared(position) >= 160000.0f) {
			if (playState != AI_GET_BALL) {
				playState = AI_GET_BALL;
				initiateDifficultySlowdown();
				makeTargetPositionInvalid();
			}
		} else if (playState != AI_SHOOT) {
			if (playState != AI_DODGE_BALL) {
				playState = AI_DODGE_BALL;
				initiateDifficultySlowdown();
				makeTargetPositionInvalid();
			}
		}
	}
}

void AIObject::decideRotation() {
	if (!difficultySlowdown) {
		switch (playState) {
			case AI_GET_BALL:
				decidePickupOrBall();
				break;
			case AI_RUN_FROM_OPPONENT:
				runFromOpponent();
				break;
			case AI_TARGET_OPPONENT:
				if (ballHoldTimer.checkTimer()) {
					shoot(BALL_TYPE_NORMAL);
				} else {
					targetOpponent();
				}
				break;
			case AI_DO_NOTHING:
			case AI_SHOOT:
				moveState = AI_MOVE_STAND;
				break;
			case AI_DODGE_BALL:
				dodgeBall();
		}
	}
}

void AIObject::checkPath(Vector2f target) {
	checkPreviousPathValidity(&target);

	if (pathPoints.size() == 0) {
		targetPosition = target;
		engine->findPath(position, target, gridMap, width, height, &pathPoints);
		if (pathPoints.size() != 0) {
			pathPoints.pop_front();	//pop the start point, AI is already at the start point
			pathPoints.push_back(Vector2i(target.x, target.y));
		}
		if (pathPoints.size() == 0) {
			return;
		}
	}

	Vector2f pointToFollow;
	pointToFollow = pathPoints.front();
	float distancePointAndAI = pointToFollow.getDistanceSquared(position);
	if (distancePointAndAI <= 900.0f) {
		pathPoints.pop_front();
	} else {
		rotateMoveVector(pointToFollow);
	}
	moveState = AI_MOVE_RUN;
}

const Competitor *AIObject::getClosestOpponent(Vector2f closestObjPos) {
	const Competitor *opponent = 0;
	float lowestDistance = 1638401.0f;
	for (int i = 0; i < opponents.size(); ++i) {
		if (!opponents[i]->isDead()) {
			float distance = opponents[i]->getPosition().getDistanceSquared(closestObjPos);
			if (distance < lowestDistance) {
				opponent = opponents[i];
				lowestDistance = distance;
			}
		}
	}

	return opponent;
}

void AIObject::targetOpponent() {
	const Competitor *opponent = getClosestOpponent(position);
	bool hasObstacle = checkForObstacles(opponent->getPosition(), ball->getWidth(), ball->getHeight());
	rotateLookVector(opponent->getPosition());

	if (pickupMove != PICKUP_PIERCING_BALL && (specialMoveCooldown || specialMove != PICKUP_PIERCING_BALL) && hasObstacle) {
		checkPath(opponent->getPosition());
	} else {
		targetPosition.x = 0;
		targetPosition.y = 0;
		targetOpponentDirectly(opponent->getPosition(), hasObstacle);
	}
}

void AIObject::targetOpponentDirectly(Vector2f opponentPosition, bool hasObstacle) {
	if (hasObstacle) {
		AIObjectLookSide lookSide;
		float distanceFromEdge, opponentDistanceFromEdge;

		lookSide = AI_RIGHT;
		distanceFromEdge = 1024.0f - position.x;
		opponentDistanceFromEdge = 1024.0f - opponentPosition.x;
		if (distanceFromEdge > position.x) {
			lookSide = AI_LEFT;
			distanceFromEdge = position.x;
			opponentDistanceFromEdge = opponentPosition.x;
		}
		if (distanceFromEdge > position.y) {
			lookSide = AI_UP;
			distanceFromEdge = position.y;
			opponentDistanceFromEdge = opponentPosition.y;
		}
		if (distanceFromEdge > 768.0f - position.y) {
			lookSide = AI_DOWN;
			distanceFromEdge = 768.0f - position.y;
			opponentDistanceFromEdge = 768.0f - opponentPosition.y;
		}

		float edgeDistanceSum = distanceFromEdge + opponentDistanceFromEdge;

		if (edgeDistanceSum <= 600.0f) {
			shootIndirectly(opponentPosition, lookSide, true);
		} else {
			shootDirectly(opponentPosition, true);
		}
	} else {
		shootDirectly(opponentPosition, false);
	}
}

void AIObject::decidePickupOrBall() {
	rotateLookVector(ball->getPosition());

	PickupItem *pickupItem = PickupItem::getInstance();

	if (pickupMove == PICKUP_NONE && pickupItem->getIsActive()) {
		if (!checkForObstacles(pickupItem->getPosition(), width, height)) {
			float pickupDistance = pickupItem->getPosition().getDistanceSquared(position);
			float ballDistance = ball->getPosition().getDistanceSquared(position);
			if (pickupDistance < ballDistance) {
				rotateMoveVector(pickupItem->getPosition());
				targetPosition = Vector2f(0.0f, 0.0f);
				moveState = AI_MOVE_RUN;
			} else {
				chaseAfterBall();
			}
		} else {
			chaseAfterBall();
		}
	} else {
		chaseAfterBall();
	}
}

void AIObject::chaseAfterBall() {
	Vector2f ballPosition = ball->getPosition();
	
	if (!hasSeenBall && checkForObstacles(ballPosition, width, height)) {
		if (hasSecondaryMove((PickupMove)BALL_TYPE_CALL_BALL) && callBall()) {
			takeAwayPickupOrSecondary((PickupMove)BALL_TYPE_CALL_BALL);
		}
		checkPath(ballPosition);
	} else {
		if (!hasSeenBall) {
			hasSeenBall = true;
			targetPosition = ballPosition;
		} else if (targetPosition != ballPosition) {
			makeTargetPositionInvalid();
			hasSeenBall = false;
			return;
		}
		const Competitor *opponent = getClosestOpponent(ballPosition);
		if (opponent && !opponent->getIsImmobilised()) {
			float distancePointAndAI = position.getDistanceSquared(ballPosition);
			float distancePointAndOpponent = opponent->getPosition().getDistanceSquared(ballPosition);
			
			if (hasSecondaryMove((PickupMove)BALL_TYPE_CALL_BALL) && callBall()) {
				takeAwayPickupOrSecondary((PickupMove)BALL_TYPE_CALL_BALL);
			}
			if (ball->getSpeed() <= BALL_CATCHABLE_SPEED && distancePointAndAI > distancePointAndOpponent && distancePointAndOpponent < cautionDistance) {
				if (hasSecondaryMove(PICKUP_IMMOBILISER)) {
					rotateLookVector(opponent->getPosition());
					shootImmobiliser();
					takeAwayPickupOrSecondary(PICKUP_IMMOBILISER);
				} else if (hasSecondaryMove(PICKUP_TELEPORT_BALL)) {
					teleportBall();
					takeAwayPickupOrSecondary(PICKUP_TELEPORT_BALL);
				} else if ((hasSecondaryMove(PICKUP_BATTERY)) && distancePointAndAI - distancePointAndOpponent <= 4900.f) {
					activateBatterySpeedUp();
					moveState = AI_MOVE_RUN;
					takeAwayPickupOrSecondary(PICKUP_BATTERY);
				} else if (!batterySpeedUp) {
					if (distancePointAndAI <= 360000.0f) {
						moveState = AI_MOVE_RUN;
						speed = -maxSpeed;
					} else {
						moveState = AI_MOVE_STAND;
					}
				} else {
					rotateMoveVector(ballPosition);
					moveState = AI_MOVE_RUN;
				}
			} else {
				rotateMoveVector(ballPosition);
				moveState = AI_MOVE_RUN;
			}
		} else {
			rotateMoveVector(ballPosition);
			moveState = AI_MOVE_RUN;
		}
	}
}

void AIObject::takeAwayPickupOrSecondary(PickupMove pickupMove) {
	if (this->pickupMove == pickupMove) {
		this->pickupMove = PICKUP_NONE;
	} else {
		secondaryAbilityCooldown = true;
		secondaryAbilityCooldownTimer.startTimer();
	}
}

void AIObject::runFromObject(const InteractiveObject *obj, bool doRayTest) {
	Vector2f objPosition = obj->getPosition();
	rotateLookVector(objPosition);

	Vector2f objectToAI = position - objPosition;
	Vector2f objectLook = obj->getLook();

	float angleSide = objectLook.getSignedAngleBetween(objectToAI);
	float unsignedAngle = abs(angleSide);

	if (doRayTest && unsignedAngle < shootingSharpness) {
		Vector2f midPoint = getMidPosition();
		Vector2f right(-lookAt.y, lookAt.x), left(lookAt.y, -lookAt.x);
		Line rightLine(midPoint, Vector2f(midPoint.x + right.x * 2000.0f, midPoint.y + right.y * 2000.0f)), 
			leftLine(midPoint, Vector2f(midPoint.x + left.x * 2000.0f, midPoint.y + left.y * 2000.0f));
		right = getClosestIntersectionPointAgainstObstacle(rightLine);
		left = getClosestIntersectionPointAgainstObstacle(leftLine);
		float rightDistance = midPoint.getDistance(right), leftDistance = midPoint.getDistance(left);
		float difference = abs(rightDistance - leftDistance); 

		if (difference <= maxSpeed) {
			moveVec = Vector2f(-lookAt.x, -lookAt.y);
		} else if (rightDistance < leftDistance) {
			moveVec = Vector2f(lookAt.y, -lookAt.x);
		} else {
			moveVec = Vector2f(-lookAt.y, lookAt.x);
		}
	} else {
		if (angleSide < 0) {
			moveVec = Vector2f(objectLook.y, -objectLook.x);
		} else {
			moveVec = Vector2f(-objectLook.y, objectLook.x);
		}
	}
		
	moveState = AI_MOVE_RUN;
}

void AIObject::dodgeBall() {
	if (!ball->getIsInvisible()) {
		float ballDistance = ball->getPosition().getDistanceSquared(position);
		if (ballDistance <= 10000.0f) {
			if (hasSecondaryMove(PICKUP_BATTERY)) { 
				activateBatterySpeedUp();
				takeAwayPickupOrSecondary(PICKUP_BATTERY);
			} else if (hasSecondaryMove(PICKUP_SHIELD)) {
				activateShield();
				takeAwayPickupOrSecondary(PICKUP_SHIELD);
			} else if (hasSecondaryMove(PICKUP_TELEPORT_BALL)) {
				teleportBall();
			}
		}
		runFromObject(ball);
	}
}

void AIObject::immobiliseThrower(const Competitor *opponent) {
	const float immobaliseBoundary = 250000.0f;
	float distance = position.getDistanceSquared(opponent->getPosition());

	if (distance <= immobaliseBoundary) {
		rotateLookVector(opponent->getPosition());
		shootImmobiliser();
		takeAwayPickupOrSecondary(PICKUP_IMMOBILISER);
	} else {
		runFromObject(opponent, true);
	}
}

void AIObject::runFromOpponent() {
	const Competitor *opponent = 0;
	for (const Competitor *competitor : opponents) {
		if (competitor->hasBall()) {
			opponent = competitor;
		}
	}

	if (hasSecondaryMove(PICKUP_IMMOBILISER) && checkForObstacles(opponent->getPosition())) {
		immobiliseThrower(opponent);
	} else {
		rotateLookVector(opponent->getPosition());

		PickupItem *pickupItem = PickupItem::getInstance();
		if (health >= COMPETITOR_START_HEALTH / 4 && pickupItem->getIsActive()) {
			Vector2f pickupPosition = pickupItem->getPosition();
			float pickupDistance = pickupPosition.getDistanceSquared(position);
			if (pickupDistance <= 40000.0f && !checkForObstacles(pickupPosition, width, height)) {
				rotateMoveVector(pickupItem->getPosition());
				moveState = AI_MOVE_RUN;
			} else {
				runFromOpponent(opponent);
			}
		} else {
			runFromOpponent(opponent);
		}
	}
}

void AIObject::runFromOpponent(const Competitor *opponent) {
	Vector2f opponentPosition = opponent->getPosition();
	float distance = position.getDistanceSquared(opponentPosition);
	if (distance > 250000.0f && checkForObstacles(opponentPosition)) {
		moveState = AI_MOVE_STAND;
	} else {
		if (opponent->getIsImmobilised() && distance <= 40000.0f) {
			Vector2f midPosition = getMidPosition();
			RayIntersection::Line backLine(midPosition, Vector2f(-lookAt.x * 2000.0f, -lookAt.y * 2000.0f));
			Vector2f closestPoint = getClosestIntersectionPointAgainstObstacle(backLine);
			if (closestPoint.getDistanceSquared(midPosition) <= 1225.0f) {
				moveVec = opponent->getLook();
				moveVec.x = -moveVec.x;
				moveVec.y = -moveVec.y;
				moveState = AI_MOVE_RUN;
			}
		} else {
			runFromObject(opponent, true);
		}
	}
}

BallType AIObject::chooseIndirectShootingMove() {
	BallType retVal;

	if (pickupMove == PICKUP_PIERCING_BALL || pickupMove == PICKUP_PINBALL || pickupMove == PICKUP_INVISIBLE_BALL) {
		retVal = (BallType)pickupMove;
		pickupMove = PICKUP_NONE;
	} else if (!specialMoveCooldown && (specialMove != PICKUP_HOMING && specialMove != PICKUP_GRANADE)) {
		retVal = (BallType)specialMove;
		specialMoveCooldown = true;
		specialMoveCooldownTimer.startTimer();
	} else {
		retVal = BALL_TYPE_NORMAL;
	}

	return retVal;
}

BallType AIObject::chooseDirectShootingMove(bool hasObstacle) {
	BallType retVal = chooseDirectShootingMoveFromPickupMove(hasObstacle);
	if (retVal == BALL_TYPE_NORMAL) {
		retVal = chooseDirectShootingMoveFromSpecialThrow(hasObstacle);
	}
	return retVal;
}

BallType AIObject::chooseDirectShootingMoveFromPickupMove(bool hasObstacle) {
	BallType retVal;

	switch (pickupMove) {
		case PICKUP_HOMING:
		case PICKUP_GRANADE:
		case PICKUP_INVISIBLE_BALL:
		case PICKUP_PINBALL:
			retVal = (BallType)pickupMove;
			pickupMove = PICKUP_NONE;
			break;
		case PICKUP_PIERCING_BALL:
			if (hasObstacle) {
				retVal = (BallType)pickupMove;
				pickupMove = PICKUP_NONE;
				break;
			}
		default:
			retVal = BALL_TYPE_NORMAL;
	}

	return retVal;
}

BallType AIObject::chooseDirectShootingMoveFromSpecialThrow(bool hasObstacle) {
	if (!specialMoveCooldown) {
		if (specialMove == PICKUP_PIERCING_BALL && !hasObstacle) {
			return BALL_TYPE_NORMAL;
		} else {
			specialMoveCooldown = true;
			specialMoveCooldownTimer.startTimer();
			return (BallType)specialMove;
		}
	}
	return BALL_TYPE_NORMAL;
}

void AIObject::shootDirectly(Vector2f opponentPosition, bool hasObstacle) {
	rotateLookVector(opponentPosition);
	if (hasObstacle) {
		BallType throwType = chooseDirectShootingMove(hasObstacle);
		if (throwType == BALL_TYPE_PIERCING) {
			shoot(throwType);
		}
	} else {
		shoot(chooseDirectShootingMove(hasObstacle));
	}
}

void AIObject::shootIndirectly(Vector2f opponentPosition, AIObjectLookSide lookSide, bool hasObstacle) {
	Vector2f targetPoint;
		
	switch (lookSide) {
		case AI_RIGHT:
			targetPoint.x = 1004.0f;
			targetPoint.y = getBounceCoordinateValueY(opponentPosition, targetPoint.x);
			break;
		case AI_LEFT:
			targetPoint.x = 20.0f;
			targetPoint.y = getBounceCoordinateValueY(opponentPosition, targetPoint.x);
			break;
		case AI_UP:
			targetPoint.y = 20.0f;
			targetPoint.x = getBounceCoordinateValueX(opponentPosition, targetPoint.y);
			break;
		case AI_DOWN:
			targetPoint.y = 748.0f;
			targetPoint.x = getBounceCoordinateValueX(opponentPosition, targetPoint.y);
	}

	//Check to see if there is any obstacle between ai and edge, and edge and oppenent
	RayIntersection::Line lineAIToTarget(position, targetPoint), lineTargetToOpponent(position, targetPoint);
	for (int i = 4; i < bBoxes->size(); ++i) {
		if (engine->checkIfLineSegmentIntersectsBB(&lineAIToTarget, &(*bBoxes)[i]) || engine->checkIfLineSegmentIntersectsBB(&lineTargetToOpponent, &(*bBoxes)[i])) {
			shootDirectly(opponentPosition, hasObstacle);
			return;
		}
	}

	rotateLookVector(targetPoint);
	shoot(chooseIndirectShootingMove());
}

void AIObject::shoot(BallType ballType) {
	ball->setLook(lookAt);
	ball->setPosition(position);
	ball->resetPreviousPosition();
	ball->setAtShootSpeed();
	ball->setTaken(false);
	ball->setShooter(this);
	ball->setBallType(ballType);
	_hasBall = false;
	isImmune = true;
	shotBall = true;
	playState = AI_SHOOT;
	pathPoints.clear();
	engine->playAudioBuffer(SfxPool::getSfxBallShoot());
}

void AIObject::move() {
	if (moveState != AI_MOVE_STAND) {
		position.x += moveVec.x * speed;
		position.y += moveVec.y * speed;
	}
}

float AIObject::getBounceCoordinateValueX(Vector2f opponentPosition, float c) {
	return (c * (position.x + opponentPosition.x) - position.x * opponentPosition.y - position.y * opponentPosition.x) / (2 * c - position.y - opponentPosition.y);
}

float AIObject::getBounceCoordinateValueX(Vector2f opponentPosition) {
	return (position.x * opponentPosition.y + position.y * opponentPosition.x) / (position.y + opponentPosition.y);
}

float AIObject::getBounceCoordinateValueY(Vector2f opponentPosition, float c) {
	return (c * (opponentPosition.y + position.y) - position.x * opponentPosition.y - position.y * opponentPosition.x) / (2 * c - opponentPosition.x - position.x);
}

float AIObject::getBounceCoordinateValueY(Vector2f opponentPosition) {
	return (position.x * opponentPosition.y + position.y * opponentPosition.x) / (opponentPosition.x + position.x);
}

void AIObject::update() {
	if (isInDeathPeriod()) {
		updateDeathPeriod();
		return;
	}

	shotImmobiliser = false;
	shotBall = false;

	updateImmunity();

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
		checkCollisionWithBall();
		checkCollisionWithImmobilisers();
		if (_hasBall) {
			checkBallHoldTimer();
		}
	} else if (difficultySlowdown) {
		difficultySlowdown = !difficultyTimer.checkTimer();
		checkCollisionWithBall();
		checkCollisionWithPickup();
	} else {
		previousPosition = position;
		decidePlayStatus();
		if (!wasCollisionWithObstacle) {
			decideRotation();
		}
		move();
		calculateBB();
		checkCollision();

		speed = (batterySpeedUp) ? maxSpeed * BATTERY_SPEED_UP : maxSpeed;	//Reset speed in case it was changed
	}
	updateShield();
	updateBatterySpeedUp();
	updateCooldownPeriod();
}

void AIObject::draw() {
	if (isInDeathPeriod()) {
		drawDeathPeriod(&sprite);
		return;
	}
	calculateRotateFromLook();

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
		if (moveState == AI_MOVE_RUN) {
			engine->draw(&spriteMoving, rotateDegree, false, false, false, position.x, position.y);
		} else {
			engine->draw(&sprite, rotateDegree, false, false, false, position.x, position.y);
		}
	}

	drawStunEffect();
	drawShieldEffect();
}

bool AIObject::isMoving() const {
	return !(moveState == AI_MOVE_STAND);
}

void AIObject::setAIDifficulty(AIDifficulty difficulty) {
	this->difficulty = difficulty;
	switch (difficulty) {
		case AI_EASY:
			difficultyTimer.changeInterval(30);
			maxSpeed /= 1.8;
			break;
		case AI_NORMAL:
			difficultyTimer.changeInterval(15);
			maxSpeed /= 1.3f;
	}
}

void AIObject::pause() {
	Competitor::pause();
	difficultyTimer.pauseTimer();
}

void AIObject::resume() {
	Competitor::resume();
	difficultyTimer.resumeTimer();
}

void AIObject::checkPreviousPathValidity(Vector2f *currentTargetPostion) {
	float targetDistanceDifference = targetPosition.getDistanceSquared(*currentTargetPostion);
	if (targetDistanceDifference > 1600.0f) {
		invalidatePath();
	}
}

void AIObject::invalidatePath() {
	pathPoints.clear();
}

bool AIObject::hasSecondaryMove(PickupMove pickupMove) {
	if (this->pickupMove == pickupMove || (!secondaryAbilityCooldown && secondaryAbility == pickupMove)) {
		return true;
	}
	return false;
}

void AIObject::rotateMoveVector(Vector2f towards) {
	moveVec = towards - position;
	moveVec.normalise();
}

void AIObject::rotateLookVector(Vector2f towards) {
	lookAt = towards - position;
	lookAt.normalise();
}

void AIObject::initiateDifficultySlowdown() {
	difficultySlowdown = true;
	difficultyTimer.startTimer();
}

void AIObject::makeTargetPositionInvalid() {
	targetPosition.x = -100.0f;
	targetPosition.y = -100.0f;
	hasSeenBall = false;
}
