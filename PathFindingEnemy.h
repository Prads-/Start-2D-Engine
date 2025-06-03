#ifndef PATH_FINDING_ENEMY_H
#define PATH_FINDING_ENEMY_H

#include "Enemy.h"
#include "Grid.h"
#include <list>
#include "Image.h"

class PathFindingEnemy : public Enemy {
protected:
	Grid **gridMap;
	std::list<Vector2i> pathPoints;
	Vector2f targetPosition;

	void invalidatePathTargetPosition();

	void findPathTowardsPlayer(Vector2f playerPosition, float distanceEpsilon, Vector2f *moveVec);
	virtual bool pointTowardsPosition(Vector2f position, float distanceEpsilon, Vector2f *moveVec);

public:
	PathFindingEnemy(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Grid** gridMap, Image *shadowSprite, int initialShadowPeriod, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites);
};

#endif