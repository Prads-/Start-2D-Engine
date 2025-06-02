#ifndef GRID_H
#define GRID_H

#include "declarations.h"
#include "vector2.h"

struct Grid {
	bool isObstacle;
};

struct GridIndexBase {
	int x, y;

	static Vector2i gridIndexToPosition(const GridIndexBase *gridIndex, int gridWidth, int gridHeight, int offsetX, int offsetY) {
		return Vector2i((gridIndex->x - 1) * gridWidth + offsetX, (gridIndex->y - 1) * gridHeight + offsetY);
	}

	static void positionToGridIndex(const Vector2f *position, int gridWidth, int gridHeight, int offsetX, int offsetY, GridIndexBase *gridIndexOut) {
		gridIndexOut->x = ((position->x - offsetX) / gridWidth) + 1;
		gridIndexOut->y = ((position->y - offsetY) / gridHeight) + 1;
	}
};

#endif