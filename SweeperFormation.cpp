#include "SweeperFormation.h"
#include "MT_RandomGenerator.h"
#include <fstream>
using namespace std;

#define RANDOM_DIRECTION			(Direction)(MT_RandomGenerator::randomNumber() % 4)
#define SWEEPER_CAUTION_DISTANCE	300.0f

SweeperFormation::SweeperFormation(StartEngine *engine, Vector2f absolutePosition, const std::vector<BoundingBox> *bBoxes,
								   Image *shadowSprite, int initialShadowPeriod, CampaignPlayer *player, BallObject *ball, Grid** gridMap, 
								   std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, Sweeper::SpritePack *spritePack) 
									: directionTimer(150), dashCooldownTimer(60) {
	this->engine = engine;
	this->bBoxes = bBoxes;
	this->absolutePostion = absolutePosition;
	this->player = player;
	isInInitialShadowPeriod = initialShadowPeriod;
	dashCooldownPeriod = false;
	dashPreparation = false;

	formation = FORMATION_ARROW;
	direction = DIRECTION_UP;
	mode = MODE_NONE;

	for (int i = 0; i < 5; ++i) {
		Sweeper *sweeper = new Sweeper(engine, absolutePosition, shadowSprite, initialShadowPeriod, player, ball, gridMap, bBoxes, immobilisers, sprites, spritePack);
		sweepers.push_back(sweeper);
		sweepersCollection.push_back(sweeper);
	}
	rearrangeSweepers();

	slots[0] = &midSlot;
	slots[1] = &rightSlots[0];
	slots[2] = &leftSlots[0];
	slots[3] = &rightSlots[1];
	slots[4] = &leftSlots[1];

	directionTimer.startTimer();
}

SweeperFormation::~SweeperFormation() {
	for (Sweeper *sweeper : sweepersCollection) {
		delete sweeper;
	}
}

void SweeperFormation::rearrangeSweepers() {
	switch (formation | direction) {
		case (FORMATION_ARROW | DIRECTION_UP):
			rearrangeArrowUp();
			break;
		case (FORMATION_ARROW | DIRECTION_RIGHT):
			rearrangeArrowRight();
			break;
		case (FORMATION_ARROW | DIRECTION_DOWN):
			rearrangeArrowDown();
			break;
		case (FORMATION_ARROW | DIRECTION_LEFT):
			rearrangeArrowLeft();
			break;
		case (FORMATION_TURTLE | DIRECTION_UP):
			rearrangeTurtleUp();
			break;
		case (FORMATION_TURTLE | DIRECTION_RIGHT):
			rearrangeTurtleRight();
			break;
		case (FORMATION_TURTLE | DIRECTION_DOWN):
			rearrangeTurtleDown();
			break;
		case (FORMATION_TURTLE | DIRECTION_LEFT):
			rearrangeTurtleLeft();
			break;
		case (FORMATION_COMPACT_TURTLE | DIRECTION_UP):
			rearrangeCompactTurtleUp();
			break;
		case (FORMATION_COMPACT_TURTLE | DIRECTION_LEFT):
			rearrangeCompactTurtleLeft();
			break;
		case (FORMATION_COMPACT_TURTLE | DIRECTION_DOWN):
			rearrangeCompactTurtleDown();
			break;
		case (FORMATION_COMPACT_TURTLE | DIRECTION_RIGHT):
			rearrangeCompactTurtleRight();
			break;
		case (FORMATION_SERPENT | DIRECTION_UP):
			rearrangeSerpentUp();
			break;
		case (FORMATION_SERPENT | DIRECTION_RIGHT):
			rearrangeSerpentRight();
			break;
		case (FORMATION_SERPENT | DIRECTION_DOWN):
			rearrangeSerpentDown();
			break;
		case (FORMATION_SERPENT | DIRECTION_LEFT):
			rearrangeSerpentLeft();
	}

	mode = MODE_TRANSFORMATION;
	transformationIsValid = true;
}

void SweeperFormation::moveSweepersInFormation() {
	float x = moveVector.x * 2.5f, y = moveVector.y * 2.5f;
	absolutePostion.x += x;
	absolutePostion.y += y;

	int counter = 0;
	for (list<Sweeper*>::iterator it = sweepers.begin(); it != sweepers.end(); ++it) {
		slots[counter]->position.x += x;
		slots[counter]->position.y += y;
		(*it)->setPosition(slots[counter]->position);
		++counter;
	}

	for (int i = 0; i < 4; ++i) {
		formationBBoxes[i].minP.x += x;
		formationBBoxes[i].minP.y += y;
		formationBBoxes[i].maxP.x += x;
		formationBBoxes[i].maxP.y += y;
	}
}

void SweeperFormation::chooseDirection() {
	Direction randomDirection = getPerpendicularRandomDirection();
	
	BoundingBox bBox = getRotationCollisionCheckBB(randomDirection);
	bool checkAgainForReverse = true;
	if (isThereCollision(bBox)) {
		randomDirection = getReverseDirection(randomDirection);
		bBox = getRotationCollisionCheckBB(randomDirection);
		if (isThereCollision(bBox)) {
			return;
		}
	}
	direction = randomDirection;
	rearrangeSweepers();
	updateSweepersLookAt();
	directionTimer.changeInterval(60 + MT_RandomGenerator::randomNumber() % 120);
	directionTimer.startTimer();
}

void SweeperFormation::rearrangeFormation(Vector2f mid, Vector2f right1, Vector2f right2, Vector2f left1, Vector2f left2) {
	midSlot.position = mid;
	rightSlots[0].position = right1;
	rightSlots[1].position = right2;
	leftSlots[0].position = left1;
	leftSlots[1].position = left2;
}

void SweeperFormation::recalculateFormationBBoxes(float bb1MinX, float bb1MinY, float bb1MaxX, float bb1MaxY,
		float bb2MinX, float bb2MinY, float bb2MaxX, float bb2MaxY,
		float bb3MinX, float bb3MinY, float bb3MaxX, float bb3MaxY,
		float bb4MinX, float bb4MinY, float bb4MaxX, float bb4MaxY) {
	formationBBoxes[0] = BoundingBox(Vector2f(absolutePostion.x + bb1MinX, absolutePostion.y + bb1MinY), Vector2f(absolutePostion.x + bb1MaxX, absolutePostion.y + bb1MaxY));
	formationBBoxes[1] = BoundingBox(Vector2f(absolutePostion.x + bb2MinX, absolutePostion.y + bb2MinY), Vector2f(absolutePostion.x + bb2MaxX, absolutePostion.y + bb2MaxY));
	formationBBoxes[2] = BoundingBox(Vector2f(absolutePostion.x + bb3MinX, absolutePostion.y + bb3MinY), Vector2f(absolutePostion.x + bb3MaxX, absolutePostion.y + bb3MaxY));
	formationBBoxes[3] = BoundingBox(Vector2f(absolutePostion.x + bb4MinX, absolutePostion.y + bb4MinY), Vector2f(absolutePostion.x + bb4MaxX, absolutePostion.y + bb4MaxY));
}

void SweeperFormation::rearrangeArrowUp() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 4, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x, absolutePostion.y + SWEEPER_HEIGHT * 2));

	moveVector = Vector2f(0.0f, -1.0f);
	recalculateFormationBBoxes(0.0f, SWEEPER_HEIGHT, SWEEPER_WIDTH, SWEEPER_HEIGHT * 2,
		SWEEPER_WIDTH, 0.0f, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT,
		SWEEPER_WIDTH * 3, 0.0f, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT,
		SWEEPER_WIDTH * 4, SWEEPER_HEIGHT, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 2);
}

void SweeperFormation::rearrangeArrowRight() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 4, absolutePostion.y + SWEEPER_HEIGHT * 2), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 4),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y));

	moveVector = Vector2f(1.0f, 0.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH * 3, 0.0f, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT, 
		SWEEPER_WIDTH * 4, SWEEPER_HEIGHT, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 2, 
		SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 4, 
		SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 5);
}

void SweeperFormation::rearrangeArrowDown() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 4), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 4, absolutePostion.y + SWEEPER_HEIGHT * 2));

	moveVector = Vector2f(0.0f, 1.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 4, 
		SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 5, 
		SWEEPER_WIDTH, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT * 5, 
		0.0f, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH, SWEEPER_HEIGHT * 4);
}

void SweeperFormation::rearrangeArrowLeft() {
	rearrangeFormation(Vector2f(absolutePostion.x, absolutePostion.y + SWEEPER_HEIGHT * 2), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 4));

	moveVector = Vector2f(-1.0f, 0.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT * 5, 
		0.0f, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH, SWEEPER_HEIGHT * 4,
		0.0f, SWEEPER_HEIGHT, SWEEPER_WIDTH, SWEEPER_HEIGHT * 2, 
		SWEEPER_WIDTH, 0.0f, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT);
}

void SweeperFormation::rearrangeTurtleUp() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 2));

	moveVector = Vector2f(0.0f, -1.0f);
	recalculateFormationBBoxes(0.0f, SWEEPER_HEIGHT, SWEEPER_WIDTH, SWEEPER_HEIGHT * 3, 
		SWEEPER_WIDTH, 0.0f, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT, 
		SWEEPER_WIDTH * 3, 0.0f, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT, 
		SWEEPER_WIDTH * 4, SWEEPER_HEIGHT, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 3);
}

void SweeperFormation::rearrangeTurtleRight() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 4, absolutePostion.y + SWEEPER_HEIGHT * 2), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT));

	moveVector = Vector2f(1.0f, 0.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH * 2, 0.0f, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT, 
		SWEEPER_WIDTH * 4, SWEEPER_HEIGHT, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 2, 
		SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 4, 
		SWEEPER_WIDTH * 2, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 5);
}

void SweeperFormation::rearrangeTurtleDown() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 4), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 2));
	
	moveVector = Vector2f(0.0f, 1.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 2, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 4, 
		SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 5, 
		SWEEPER_WIDTH, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT * 5, 
		0.0f, SWEEPER_HEIGHT * 2, SWEEPER_WIDTH, SWEEPER_HEIGHT * 4);
}

void SweeperFormation::rearrangeTurtleLeft() {
	rearrangeFormation(Vector2f(absolutePostion.x, absolutePostion.y + SWEEPER_HEIGHT * 2), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 3));

	moveVector = Vector2f(-1.0f, 0.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 5, 
		0.0f, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH, SWEEPER_HEIGHT * 4, 
		0.0f, SWEEPER_HEIGHT, SWEEPER_WIDTH, SWEEPER_HEIGHT * 2, 
		SWEEPER_WIDTH, 0.0f, SWEEPER_WIDTH * 3, SWEEPER_HEIGHT);
}

void SweeperFormation::rearrangeCompactTurtleUp() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2.5f, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2.5f, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 1.5f, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 1.5f, absolutePostion.y + SWEEPER_HEIGHT * 2));

	moveVector = Vector2f(0.0f, -1.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH, 0.0f, SWEEPER_WIDTH * 1.5f, SWEEPER_HEIGHT * 3, 
		SWEEPER_WIDTH * 1.5f, 0.0f, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT, 
		SWEEPER_WIDTH * 3, 0.0f, SWEEPER_WIDTH * 3.5f, SWEEPER_HEIGHT, 
		SWEEPER_WIDTH * 3.5f, 0.0f, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 3);
}

void SweeperFormation::rearrangeCompactTurtleRight() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 4, absolutePostion.y + SWEEPER_HEIGHT * 2), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 2.5f),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 2.5f),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 1.5f),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 1.5f));

	moveVector = Vector2f(1.0f, 0.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH * 2, SWEEPER_HEIGHT, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 1.5f, 
		SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 1.5f, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 2, 
		SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 3.5f, 
		SWEEPER_WIDTH * 2, SWEEPER_HEIGHT * 3.5f, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 4);
}

void SweeperFormation::rearrangeCompactTurtleDown() {
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 4), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 1.5f, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 1.5f, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2.5f, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2.5f, absolutePostion.y + SWEEPER_HEIGHT * 2));
	
	moveVector = Vector2f(0.0f, 1.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH * 3.5f, SWEEPER_HEIGHT * 2, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 5, 
		SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 3.5, SWEEPER_HEIGHT * 5, 
		SWEEPER_WIDTH * 1.5f, SWEEPER_HEIGHT * 4, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT * 5, 
		SWEEPER_WIDTH, SWEEPER_HEIGHT * 2, SWEEPER_WIDTH * 1.5f, SWEEPER_HEIGHT * 5);
}

void SweeperFormation::rearrangeCompactTurtleLeft() {
	rearrangeFormation(Vector2f(absolutePostion.x, absolutePostion.y + SWEEPER_HEIGHT * 2), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 1.5f),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 1.5f),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 2.5f),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 2.5f));

	moveVector = Vector2f(-1.0f, 0.0f);
	recalculateFormationBBoxes(0.0f, SWEEPER_HEIGHT * 3.5f, SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 4, 
		0.0f, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH, SWEEPER_HEIGHT * 3.5, 
		0.0f, SWEEPER_HEIGHT * 1.5f, SWEEPER_WIDTH, SWEEPER_HEIGHT * 2, 
		0.0f, SWEEPER_HEIGHT, SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 1.5f);
}

void SweeperFormation::rearrangeSerpentUp() {
	absolutePostion.y -= SWEEPER_HEIGHT * 2.0f;
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 4));
	
	moveVector = Vector2f(0.0f, -1.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH, 0.0f, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT * 5, 
		SWEEPER_WIDTH * 3, 0.0f, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 5, 
		0.0f, SWEEPER_HEIGHT * 2, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 3, 
		0.0f, SWEEPER_HEIGHT * 2, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 3);
}

void SweeperFormation::rearrangeSerpentRight() {
	absolutePostion.x += SWEEPER_WIDTH * 2.0f;
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 4, absolutePostion.y + SWEEPER_HEIGHT * 2), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x, absolutePostion.y + SWEEPER_HEIGHT * 2));

	moveVector = Vector2f(1.0f, 0.0f);
	recalculateFormationBBoxes(0.0f, SWEEPER_HEIGHT, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 2, 
		0.0f, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 4, 
		SWEEPER_WIDTH * 2, 0.0f, SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 5, 
		SWEEPER_WIDTH * 2, 0.0f, SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 5);
}

void SweeperFormation::rearrangeSerpentDown() {
	absolutePostion.y += SWEEPER_HEIGHT * 2.0f;
	rearrangeFormation(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 4), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 3),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y));
	
	moveVector = Vector2f(0.0f, 1.0f);
	recalculateFormationBBoxes(SWEEPER_WIDTH * 3, 0.0f, SWEEPER_WIDTH * 4, SWEEPER_HEIGHT * 5, 
		SWEEPER_WIDTH, 0.0f, SWEEPER_WIDTH * 2, SWEEPER_HEIGHT * 5,
		0.0f, SWEEPER_HEIGHT * 2, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 3,
		0.0f, SWEEPER_HEIGHT * 2, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 3);
}

void SweeperFormation::rearrangeSerpentLeft() {
	absolutePostion.x -= SWEEPER_WIDTH * 2.0f;
	rearrangeFormation(Vector2f(absolutePostion.x, absolutePostion.y + SWEEPER_HEIGHT * 2), 
		Vector2f(absolutePostion.x + SWEEPER_WIDTH, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2, absolutePostion.y + SWEEPER_HEIGHT * 2),
		Vector2f(absolutePostion.x + SWEEPER_WIDTH * 4, absolutePostion.y + SWEEPER_HEIGHT * 2));

	moveVector = Vector2f(-1.0f, 0.0f);
	recalculateFormationBBoxes(0.0f, SWEEPER_HEIGHT * 3, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 4, 
		0.0f, SWEEPER_HEIGHT, SWEEPER_WIDTH * 5, SWEEPER_HEIGHT * 2,
		SWEEPER_WIDTH * 2, 0.0f, SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 5,
		SWEEPER_WIDTH * 2, 0.0f, SWEEPER_WIDTH * 3, SWEEPER_HEIGHT * 5);
}

void SweeperFormation::checkCollision() {
	switch (formation) {
		case FORMATION_ARROW:
			checkArrowCollision();
			break;
		case FORMATION_TURTLE:
			checkTurtleCollision();
			break;
		case FORMATION_SERPENT:
			checkSerpentCollision();
			break;
		case FORMATION_COMPACT_TURTLE:
			checkCompactTurtleCollision();
	}
}

void SweeperFormation::checkArrowCollision() {
	if (checkMidSlotCollision()) {
		return;
	}

	size_t numberOfSweepers = sweepers.size();

	if (numberOfSweepers > 3) {
		for (const BoundingBox &obstacle : *bBoxes) {
			if ((numberOfSweepers == 5 && engine->checkCollision(&obstacle, &formationBBoxes[0])) || engine->checkCollision(&obstacle, &formationBBoxes[3])) {
				formation = FORMATION_TURTLE;
				rearrangeSweepers();
				return;
			}
		}
	}

	for (const BoundingBox &obstacle : *bBoxes) {
		if ((numberOfSweepers > 2 && engine->checkCollision(&obstacle, &formationBBoxes[1])) || engine->checkCollision(&obstacle, &formationBBoxes[2])) {
			formation = FORMATION_COMPACT_TURTLE;
			rearrangeSweepers();
			return;
		}
	}
}

void SweeperFormation::checkTurtleCollision() {
	if (checkMidSlotCollision()) {
		return;
	}

	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, &formationBBoxes[1]) || engine->checkCollision(&obstacle, &formationBBoxes[2])) {
			formation = FORMATION_COMPACT_TURTLE;
			rearrangeSweepers();
			return;
		}
	}

	bool wasCollision = false;
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, &formationBBoxes[0]) || engine->checkCollision(&obstacle, &formationBBoxes[3])) {
			wasCollision = true;
			break;
		}
	}
	if (!wasCollision) {
		formation = FORMATION_ARROW;
		rearrangeSweepers();
	}
}

void SweeperFormation::checkCompactTurtleCollision() {
	if (checkMidSlotCollision()) {
		return;
	}

	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, &formationBBoxes[1]) || engine->checkCollision(&obstacle, &formationBBoxes[2])) {
			formation = FORMATION_SERPENT;
			rearrangeSweepers();
			return;
		}
	}

	bool wasCollision = false;
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, &formationBBoxes[0]) || engine->checkCollision(&obstacle, &formationBBoxes[3])) {
			wasCollision = true;
			break;
		}
	}
	if (!wasCollision) {
		formation = (sweepers.size() > 3) ? FORMATION_TURTLE : FORMATION_ARROW;
		rearrangeSweepers();
	}
}

void SweeperFormation::checkSerpentCollision() {
	bool wasCollision = false;
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, &formationBBoxes[0]) || engine->checkCollision(&obstacle, &formationBBoxes[1])) {
			wasCollision = true;
			break;
		}
	}
	if (!wasCollision) {
		formation = FORMATION_COMPACT_TURTLE;
		rearrangeSweepers();
		return;
	}

	checkMidSlotCollision();
}

SweeperFormation::Direction SweeperFormation::getReverseDirection(Direction currentDirection) {
	switch (currentDirection) {
		case DIRECTION_UP:
			return DIRECTION_DOWN;
		case DIRECTION_RIGHT:
			return DIRECTION_LEFT;
		case DIRECTION_DOWN:
			return DIRECTION_UP;
		default:
			return DIRECTION_RIGHT;
	}
}

void SweeperFormation::reverseDirection() {
	direction = getReverseDirection(direction);
	rearrangeSweepers();
	updateSweepersLookAt();
}

bool SweeperFormation::checkMidSlotCollision() {
	const BoundingBox *midSweeperBBox = (*sweepers.begin())->getBoundingBox();
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&obstacle, midSweeperBBox)) {
			reverseDirection();
			return true;
		}
	}
	return false;
}

void SweeperFormation::doTransformation() {
	bool transformationComplete = true;

	int counter = 0;
	for (Sweeper *sweeper : sweepers) {
		if (transformationComplete) {
			transformationComplete = sweeper->moveTowardsPosition(slots[counter++]->position);
		} else {
			sweeper->moveTowardsPosition(slots[counter++]->position);
		}

	}

	if (transformationComplete) {
		mode = MODE_NONE;
	}
}

void SweeperFormation::update() {
	if (isInInitialShadowPeriod) {
		updateShadowPeriod();
		directionTimer.startTimer();
		return;
	}

	if (sweepers.size() != 0) {
		if (mode == MODE_FINAL_DASH) {
			updateFinalDash();
		} else {
			updateFormation();
		}
	}
	updateDeadSweepers();
}

void SweeperFormation::draw() {
	if (sweepers.size() != 0) {
		if (mode == MODE_FINAL_DASH) {
			dasher->draw();
		} else {
			drawSweepers();
		}
	}
	drawDeadSweepers();
}

void SweeperFormation::drawSweepers() {
	for (Sweeper *sweeper : sweepers) {
		sweeper->draw();
	}
}

void SweeperFormation::checkRotation() {
	if (directionTimer.checkTimer()) {
		chooseDirection();
	}
}

BoundingBox SweeperFormation::getRotationCollisionCheckBB(Direction direction) const {
	switch (direction) {
		case DIRECTION_UP:
			return BoundingBox(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2.0f, absolutePostion.y - SWEEPER_HEIGHT * 2.0f), Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3.0f, absolutePostion.y + SWEEPER_HEIGHT * 3.0f));
		case DIRECTION_RIGHT:
			return BoundingBox(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2.0f, absolutePostion.y + SWEEPER_HEIGHT * 2.0f), Vector2f(absolutePostion.x + SWEEPER_WIDTH * 7.0f, absolutePostion.y + SWEEPER_HEIGHT * 3.0f));
		case DIRECTION_DOWN:
			return BoundingBox(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2.0f, absolutePostion.y + SWEEPER_HEIGHT * 2.0f), Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3.0f, absolutePostion.y + SWEEPER_HEIGHT * 7.0f));
		default:
			return BoundingBox(Vector2f(absolutePostion.x - SWEEPER_WIDTH * 2.0f, absolutePostion.y + SWEEPER_HEIGHT * 2.0f), Vector2f(absolutePostion.x + SWEEPER_WIDTH * 3.0f, absolutePostion.y + SWEEPER_HEIGHT * 3.0f));
	}
}

SweeperFormation::Direction SweeperFormation::getPerpendicularRandomDirection() {
	int randomNumber = MT_RandomGenerator::randomNumber() % 2;

	switch (direction) {
		case DIRECTION_UP:
			if (randomNumber) {
				return DIRECTION_RIGHT;
			} else {
				return DIRECTION_LEFT;
			}
			break;
		case DIRECTION_RIGHT:
			if (randomNumber) {
				return DIRECTION_DOWN;
			} else {
				return DIRECTION_UP;
			}
			break;
		case DIRECTION_DOWN:
			if (randomNumber) {
				return DIRECTION_LEFT;
			} else {
				return DIRECTION_RIGHT;
			}
			break;
		default:
			if (randomNumber) {
				return DIRECTION_UP;
			} else {
				return DIRECTION_DOWN;
			}
	}
}

void SweeperFormation::updateDash() {
	if (dasher->performDash()) {
		stopDash();
	}
}

void SweeperFormation::checkInitiateDash() {
	if (dashCooldownPeriod) {
		if (dashCooldownTimer.checkTimer()) {
			dashCooldownPeriod = false;
		}
	} else {
		float distance = player->getPosition().getDistanceSquared(Vector2f(absolutePostion.x + SWEEPER_WIDTH * 2.5f, absolutePostion.y + SWEEPER_HEIGHT * 2.5f));
		if (distance < 22500.0f) {
			getRandomDasher();
			dasher->prepareToDash();
			dashPreparation = true;
		}
	}
}

void SweeperFormation::initiateDash() {
	dasher->initiateDash();
	mode = MODE_DASH;
	dashPreparation = false;
}

void SweeperFormation::getRandomDasher() {
	list<Sweeper*>::iterator it = sweepers.begin();
	for (int randomSweeperIndex = MT_RandomGenerator::randomNumber() % sweepers.size(); randomSweeperIndex != 0; --randomSweeperIndex) {
		++it;
	}
	dasher = *it;
}

void SweeperFormation::checkSweepersCollisionWithPlayerAndBall() {
	list<Sweeper*>::iterator it = sweepers.begin();
	while (it != sweepers.end()) {
		(*it)->checkCollisionWithPlayer();
		(*it)->checkCollisionWithBall();
		if ((*it)->isDead()) {
			if (dasher == *it) {
				stopDash();
			}
			deadSweepers.push_back(*it);
			it = sweepers.erase(it);
		} else {
			++it;
		}
	}
}

void SweeperFormation::stopDash() {
	dasher->makeModeNormal();
	dasher->setLook(moveVector);
	dasher = 0;
	dashCooldownPeriod = true;
	dashPreparation = false;
	mode = MODE_TRANSFORMATION;
	dashCooldownTimer.startTimer();
}

void SweeperFormation::updateFormation() {
	if (sweepers.size() == 1) {
		if (mode == MODE_TRANSFORMATION) {
			doTransformation();
		} else {
			dasher = *sweepers.begin();
			dasher->coolDown();
			if (dasher->isThereCollision()) {
				dasher->setAtRandomPosition();
			}
			mode = MODE_FINAL_DASH;
		}
		checkSweepersCollisionWithPlayerAndBall();
		return;
	}
	
	switch (mode) {
		case MODE_NONE:
			moveSweepersInFormation();
			if (dashPreparation) {
				if (dasher->updateDashPreparation()) {
					initiateDash();
				}
			} else {
				checkInitiateDash();
			}
			break;
		case MODE_DASH:
			updateDash();
			break;
		default:
			doTransformation();
	}

	if (mode != MODE_DASH) {
		if (mode != MODE_TRANSFORMATION) {
			checkCollision();
		}
		checkRotation();
	}

	checkSweepersCollisionWithPlayerAndBall();
}

void SweeperFormation::updateFinalDash() {
	dasher->update();	
	if (dasher->isDead()) {
		deadSweepers.push_back(dasher);
		sweepers.clear();
	}
}

bool SweeperFormation::isDead() {
	return (sweepers.size() == 0 && deadSweepers.size() == 0);
}

BoundingBox SweeperFormation::getBoundingBox() const {
	return BoundingBox(absolutePostion, Vector2f(absolutePostion.x + SWEEPER_WIDTH * 5.0f, absolutePostion.y + SWEEPER_HEIGHT * 5.0f));
}

void SweeperFormation::updateShadowPeriod() {
	for (Sweeper *sweeper : sweepers) {
		sweeper->checkShadowPeriod();
	}
	for (const Sweeper *sweeper : sweepers) {
		if (sweeper->getIsInInitialShadowPeriod()) {
			return;
		}
	}
	isInInitialShadowPeriod = false;
}

void SweeperFormation::getEnemiesPointer(std::vector<const Enemy*> *enemiesPtrOut) {
	for (Sweeper *sweeper : sweepers) {
		enemiesPtrOut->push_back(sweeper);
	}
}

void SweeperFormation::setInRandomPosition() {
	while (true) {
		absolutePostion.x = 80.0f + MT_RandomGenerator::randomNumber() % 864;
		absolutePostion.y = 80.0f + MT_RandomGenerator::randomNumber() % 608;
		
		BoundingBox bBox(Vector2f(absolutePostion.x, absolutePostion.y), Vector2f(absolutePostion.x + 100.0f, absolutePostion.y + 100.0f));
		if (!isThereCollision(bBox)) {
			break;
		}
	}
	direction = DIRECTION_UP;
	formation = FORMATION_ARROW;
	rearrangeSweepers();
	for (Sweeper *sweeper : sweepers) {
		sweeper->setPosition(absolutePostion);
	}
}

bool SweeperFormation::isThereCollision(BoundingBox bBox) {
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&bBox, &obstacle)) {
			return true;
		}
	}
	return false;
}

void SweeperFormation::pause() {
	for (Sweeper *sweeper : sweepers) {
		sweeper->pause();
	}
	directionTimer.pauseTimer();
	dashCooldownTimer.pauseTimer();
}

void SweeperFormation::resume() {
	for (Sweeper *sweeper : sweepers) {
		sweeper->resume();
	}
	directionTimer.resumeTimer();
	dashCooldownTimer.resumeTimer();
}

void SweeperFormation::updateDeadSweepers() {
	for (list<Sweeper*>::iterator it = deadSweepers.begin(); it != deadSweepers.end();) {
		if ((*it)->checkDeathTimer()) {
			it = deadSweepers.erase(it);
		} else {
			++it;
		}
	}
}

void SweeperFormation::drawDeadSweepers() {
	for (Sweeper *sweeper : deadSweepers) {
		sweeper->draw();
	}
}

void SweeperFormation::updateSweepersLookAt() {
	for (Sweeper *sweeper : sweepers) {
		sweeper->setLook(moveVector);
	}
}