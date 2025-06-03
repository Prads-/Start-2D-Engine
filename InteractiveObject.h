#ifndef INTERACTIVE_OBJECT_H
#define INTERACTIVE_OBJECT_H

#include "declarations.h"
#include "GameObject.h"
#include "BoundingBox.h"
#include "StartEngine.h"

class InteractiveObject : public GameObject {
protected:
	enum CollisionSide {
		TOP, BOTTOM, LEFT, RIGHT
	};

	Vector2f position, previousPosition, lookAt, startLookAt;
	BoundingBox bBox;
	int width, height;
	int rotateDegree;
	float speed;
	const std::vector<BoundingBox> *bBoxes;

	void calculateBB();
	CollisionSide getCollisionSide(const BoundingBox *bBox);

public:
	InteractiveObject(StartEngine *engine);
	InteractiveObject(StartEngine *engine, const std::vector<BoundingBox> *bBoxes);

	bool moveTowardsPosition(Vector2f position);
	void moveTowardsDirection(Vector2f direction);
	
	float getSpeed() const;
	void setSpeed(float speed);

	Vector2f getPosition() const;
	Vector2f getMidPosition() const;
	void resetPreviousPosition();
	virtual void setPosition(Vector2f position);
	void setMidPosition(Vector2f midPosition);

	void setStartLook(Vector2f startLookAt);
	Vector2f getLook() const;
	virtual void setLook(Vector2f look);

	const BoundingBox *getBoundingBox() const;
	void setBBoxes(const std::vector<BoundingBox> *bBoxes);

	virtual void update() = 0;
	virtual void draw() = 0;
	virtual bool isMoving() const;
	virtual void setAtRandomPosition();

	void setAtRandomPositionRelativeWithinRadius(Vector2f relativePosition, int radius);
	bool isThereCollision();
	virtual void reverseLookAt();
	void setAtPreviousPosition();

	void calculateLookFromRotateDegree();
	void calculateRotateFromLook();
	int getRotateDegree() const;

	int getWidth() const;
	int getHeight() const;
};

#endif