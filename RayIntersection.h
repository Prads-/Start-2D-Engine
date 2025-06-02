#ifndef RAY_INTERSECTION_H
#define RAY_INTERSECTION_H

#include "declarations.h"
#include "BoundingBox.h"
#include "vector2.h"

namespace RayIntersection {
	struct Line {
		Vector2f p1, p2;

		Line() { }

		Line(Vector2f p1, Vector2f p2) {
			this->p1 = p1;
			this->p2 = p2;
		}
		
		Vector2f getVector() const {
			return Vector2f(p2.x - p1.x, p2.y - p1.y);
		}
	};

	bool checkIfLineSegmentIntersectsBB(const Line *line, const BoundingBox *bBox);
	bool getLineIntersectionPoint(const Line *l1, const Line *l2, Vector2f *pOut);
}

#endif