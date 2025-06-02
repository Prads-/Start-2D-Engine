#ifndef COLLISION_H
#define COLLISION_H

#include "declarations.h"
#include "vector2.h"
#include "BoundingBox.h"
#include "SAT_BoundingBox.h"

namespace Collision {
	bool testCollisionBottom(const BoundingBox *bObject, const BoundingBox *bBox);
	bool testCollisionTop(const BoundingBox *bObject, const BoundingBox *bBox);
	bool testCollision(const BoundingBox *b1, const BoundingBox *b2);
	bool testCollisionSAT(const SATBoundingBox *b1, const SATBoundingBox *b2);

	bool testCollision(const Vector2f *p1, const Vector2f *p2, float radius1, float radius2);
}

#endif