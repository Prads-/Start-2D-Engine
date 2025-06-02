#include "RayIntersection.h"

static void getParameters(const RayIntersection::Line *l1, const RayIntersection::Line *l2, float &t, float &u);

bool RayIntersection::checkIfLineSegmentIntersectsBB(const Line *line, const BoundingBox *bBox) {
	Line d1(bBox->minP, bBox->maxP), d2(Vector2f(bBox->maxP.x, bBox->minP.y), Vector2f(bBox->minP.x, bBox->maxP.y));

	float t, u;
	getParameters(line, &d1, t, u);
	if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
		return true;
	}
	getParameters(line, &d2, t, u);
	if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
		return true;
	}
	
	return false;
}

bool RayIntersection::getLineIntersectionPoint(const Line *l1, const Line *l2, Vector2f *pOut) {
	float t, u;
	getParameters(l1, l2, t, u);
	if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
		Vector2f v = l1->getVector();
		pOut->x = l1->p1.x + v.x * t;
		pOut->y = l1->p1.y + v.y * t;
		return true;
	}
	return false;
}

static void getParameters(const RayIntersection::Line *l1, const RayIntersection::Line *l2, float &t, float &u) {
	Vector2f vL1 = l1->getVector(), vL2 = l2->getVector();

	float divisor = (vL2.x * vL1.y - vL2.y * vL1.x);
	if (divisor == 0.0f) {
		u = -1.0f;
		t = -1.0f;
	} else {
		u = (vL1.x * l2->p1.y - l1->p1.y * vL1.x - vL1.y * l2->p1.x + l1->p1.x * vL1.y) / divisor;
		if (vL1.x == 0.0f) {
			if (vL1.y == 0.0f) {
				t = -1.0f;
			} else {
				t = (l2->p1.y + u * vL2.y - l1->p1.y) / vL1.y;
			}
		} else {
			t = (l2->p1.x + u * vL2.x - l1->p1.x) / vL1.x;
		}
	}
}