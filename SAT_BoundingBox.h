#ifndef SAT_BOUNDING_BOX_H
#define SAT_BOUNDING_BOX_H

#include "vector2.h"

struct SATBoundingBox {
	Vector2f p[4];

	SATBoundingBox(Vector2f topLeft, Vector2f topRight, Vector2f bottomRight, Vector2f bottomLeft) {
		this->p[0] = topLeft;
		this->p[1] = topRight;
		this->p[2] = bottomRight;
		this->p[3] = bottomLeft;
	}
};

#endif