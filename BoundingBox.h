#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "declarations.h"
#include "vector2.h"

struct BoundingBox {
	Vector2f minP, maxP;

	BoundingBox() { }

	BoundingBox(Vector2f minP, Vector2f maxP) {
		this->minP = minP;
		this->maxP = maxP;
	}
};

#endif