#include "Collision.h"

static Vector2f projectVector(const Vector2f *v, const Vector2f *to);

bool Collision::testCollisionTop(const BoundingBox *bObject, const BoundingBox *bPath) {
	Vector2i topCenter((bObject->maxP.x + bObject->minP.x) / 2, bObject->minP.y);

	if (topCenter.y <= bPath->maxP.y && topCenter.y >= bPath->minP.y && topCenter.x >= bPath->minP.x && topCenter.x <= bPath->maxP.x) {
		return true;
	}
	return false;
}

bool Collision::testCollisionBottom(const BoundingBox *bObject, const BoundingBox *bPath) {
	Vector2i bottomCenter((bObject->maxP.x + bObject->minP.x) / 2, bObject->maxP.y + 1);

	if (bottomCenter.y <= bPath->maxP.y && bottomCenter.y >= bPath->minP.y && bottomCenter.x >= bPath->minP.x && bottomCenter.x <= bPath->maxP.x) {
		return true;
	}
	return false;
}

bool Collision::testCollision(const BoundingBox *b1, const BoundingBox *b2) {
	if (b1->minP.x <= b2->maxP.x && b1->maxP.x >= b2->minP.x && b1->maxP.y >= b2->minP.y && b1->minP.y <= b2->maxP.y) {
		return true;
	}
	return false;
}

bool Collision::testCollisionSAT(const SATBoundingBox *b1, const SATBoundingBox *b2) {
	Vector2f axis[4];

	axis[0] = b1->p[0] - b1->p[1];
	axis[1] = b1->p[0] - b1->p[3];
	axis[2] = b2->p[0] - b2->p[1];
	axis[3] = b2->p[0] - b2->p[3];

	float min1, max1, min2, max2;
	Vector2f projectedPoint;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			projectedPoint = projectVector(&b1->p[j], &axis[i]);
			float scalar = projectedPoint * axis[i];
			if (j == 0) {
				min1 = max1 = scalar;
			} else if (scalar < min1) {
				min1 = scalar;
			} else if (scalar > max1) {
				max1 = scalar;
			}
			projectedPoint = projectVector(&b2->p[j], &axis[i]);
			scalar = projectedPoint * axis[i];
			if (j == 0) {
				min2 = max2 = scalar;
			} else if (scalar < min2) {
				min2 = scalar;
			} else if (scalar > max2) {
				max2 = scalar;
			}
		}
		if (min2 <= max1 && max2 >= min1) {
			continue;
		} else {
			return false;
		}
	}
	return true;
}

static Vector2f projectVector(const Vector2f *v, const Vector2f *to) {
	Vector2f retVal;

	retVal.x = ((*v * *to) / (to->x * to->x + to->y * to->y)) * to->x;
	retVal.y = ((*v * *to) / (to->x * to->x + to->y * to->y)) * to->y;

	return retVal;
}

bool Collision::testCollision(const Vector2f *p1, const Vector2f *p2, float radius1, float radius2) {
	float radiusSum = radius1 + radius2;
	if (p1->getDistance(*p2) <= radiusSum) {
		return true;
	}
	return false;
}