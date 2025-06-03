#include "PathFindingEnemy.h"

PathFindingEnemy::PathFindingEnemy(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Grid** gridMap, Image *shadowSprite, int initialShadowPeriod, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites)
				: Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriod, immobilisers, sprites) {
	this->gridMap = gridMap;
	targetPosition = Vector2f(0.0f, 0.0f);
}

void PathFindingEnemy::findPathTowardsPlayer(Vector2f playerPosition, float distanceEpsilon, Vector2f *moveVec) {
	setPosition(Vector2f(this->position.x + moveVec->x * speed, this->position.y + moveVec->y * speed));
	
	float distance = targetPosition.getDistanceSquared(playerPosition);

	if (distance > 22500.0f) {
		pathPoints.clear();
		engine->findPath(position, playerPosition, gridMap, width, height, &pathPoints);
		targetPosition = playerPosition;
	}
	if (pathPoints.size() != 0) {
		Vector2f point;
		point = pathPoints.front();

		if (pointTowardsPosition(point, distanceEpsilon, moveVec)) {
			pathPoints.pop_front();
		}
	} else {
		invalidatePathTargetPosition();
	}
}

bool PathFindingEnemy::pointTowardsPosition(Vector2f position, float distanceEpsilon, Vector2f *moveVec) {
	float distance = position.getDistanceSquared(this->position);
	
	if (distance > distanceEpsilon) {
		*moveVec = position - this->position;
		moveVec->normalise();
		return false;
	} else {
		return true;
	}
}

void PathFindingEnemy::invalidatePathTargetPosition() {
	targetPosition.x = 9999.0f;
	targetPosition.y = 9999.0f;
}