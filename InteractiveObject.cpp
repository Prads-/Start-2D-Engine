#include "InteractiveObject.h"
#include "MT_RandomGenerator.h"

InteractiveObject::InteractiveObject(StartEngine *engine) : GameObject(engine) {
	rotateDegree = 0;
}

InteractiveObject::InteractiveObject(StartEngine *engine, const std::vector<BoundingBox> *bBoxes) : GameObject(engine) { 
	rotateDegree = 0;
	this->bBoxes = bBoxes;
}

Vector2f InteractiveObject::getPosition() const {
	return position;
}

void InteractiveObject::calculateBB() {
	bBox.minP = position;
	bBox.maxP = Vector2i(position.x + width, position.y + height);
}

InteractiveObject::CollisionSide InteractiveObject::getCollisionSide(const BoundingBox *bBox) {
	if (previousPosition.x + width < bBox->minP.x || previousPosition.x > bBox->maxP.x) {
		if (previousPosition.x > bBox->maxP.x) {
			return LEFT;
		} else {
			return RIGHT;
		}
	} else {
		if (previousPosition.y > bBox->maxP.y) {
			return TOP;
		} else {
			return BOTTOM;
		}
	}
}

void InteractiveObject::calculateLookFromRotateDegree() {
	lookAt = startLookAt.getRotate(engine->sin(rotateDegree), engine->cos(rotateDegree));
}

void InteractiveObject::calculateRotateFromLook() {
	rotateDegree = lookAt.getSignedAngleBetweenInDegrees(Vector2f(0.0f, -1.0f));
}

int InteractiveObject::getRotateDegree() const {
	return rotateDegree;
}

int InteractiveObject::getWidth() const {
	return width;
}

int InteractiveObject::getHeight() const {
	return height;
}

void InteractiveObject::setPosition(Vector2f position) {
	this->position = position;
	calculateBB();
}

void InteractiveObject::setMidPosition(Vector2f midPosition) {
	this->position = Vector2f(midPosition.x - width / 2, midPosition.y - height / 2);
	calculateBB();
}

void InteractiveObject::setStartLook(Vector2f startLookAt) {
	this->startLookAt = startLookAt;
	lookAt = startLookAt;
}

Vector2f InteractiveObject::getLook() const {
	return lookAt;
}

void InteractiveObject::setLook(Vector2f look) {
	lookAt = look;
	float angle = look.getSignedAngleBetween(startLookAt);
	angle *= -180.0f / _FLOAT_PI;
	if (angle < 0.0f) {
		angle = 360.0f + angle;
	}
	rotateDegree = angle;
}

const BoundingBox *InteractiveObject::getBoundingBox() const {
	return &bBox;
}

void InteractiveObject::setBBoxes(const std::vector<BoundingBox> *bBoxes) {
	this->bBoxes = bBoxes;
}

bool InteractiveObject::isMoving() const {
	return false;
}

void InteractiveObject::setAtRandomPosition() {
	do {
		position.x = MT_RandomGenerator::randomNumber() % FRAME_WIDTH;
		position.y = MT_RandomGenerator::randomNumber() % FRAME_HEIGHT;
		calculateBB();
	} while (isThereCollision());
}

void InteractiveObject::setAtRandomPositionRelativeWithinRadius(Vector2f relativePosition, int radius) {
	int diameter = radius * 2;
	int offsetX, offsetY;
	bool isOutsideFrame;
	do {
		offsetX = (MT_RandomGenerator::randomNumber() % diameter) - radius;
		offsetY = (MT_RandomGenerator::randomNumber() % diameter) - radius;
		position.x = relativePosition.x + offsetX;
		position.y = relativePosition.y + offsetY;
		if (position.x < 0 || position.x >= FRAME_WIDTH || position.y < 0 || position.y >= FRAME_HEIGHT) {
			isOutsideFrame = true;
			continue;
		} else {
			isOutsideFrame = false;
		}
		calculateBB();
	} while (isOutsideFrame || isThereCollision());
}

float InteractiveObject::getSpeed() const {
	return speed;
}

bool InteractiveObject::isThereCollision() {
	for (const BoundingBox &obstacle : *bBoxes) {
		if (engine->checkCollision(&bBox, &obstacle)) {
			return true;
		}
	}
	return false;
}

Vector2f InteractiveObject::getMidPosition() const {
	return Vector2f(position.x + width / 2.0f, position.y + height / 2.0f);
}

void InteractiveObject::resetPreviousPosition() {
	previousPosition = position;
}

bool InteractiveObject::moveTowardsPosition(Vector2f position) {
	float distance = position.getDistance(this->position);

	if (distance > speed) {
		lookAt = position - this->position;
		lookAt.normalise();

		setPosition(Vector2f(this->position.x + lookAt.x * speed, this->position.y + lookAt.y * speed));
		return false;
	} else {
		setPosition(position);
		return true;
	}
}

void InteractiveObject::setSpeed(float speed) {
	this->speed = speed;
}

void InteractiveObject::moveTowardsDirection(Vector2f direction) {
	lookAt = direction;
	setPosition(Vector2f(position.x + lookAt.x * speed, position.y + lookAt.y * speed));
}

void InteractiveObject::reverseLookAt() {
	lookAt = Vector2f(-lookAt.x, -lookAt.y);
}

void InteractiveObject::setAtPreviousPosition() {
	position = previousPosition;
}