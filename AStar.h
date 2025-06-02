#ifndef PATH_FIND_ASTAR_H
#define PATH_FIND_ASTAR_H

#include "build.h"
#include "declarations.h"
#include <stdlib.h>
#include <list>
#include "Grid.h"
#include "vector2.h"

class PathFinderAStar {
private:
	struct GridIndex : public GridIndexBase {
		int f, g;
		GridIndex *predecessor;

		GridIndex() {
			predecessor = 0;
		}

		GridIndex(int x, int y, int g, GridIndex *predecessor) {
			this->x = x;
			this->y = y;
			this->g = g;
			this->predecessor = predecessor;
		}

		int getManhattanDistance(const GridIndex &gI) const {
			return abs(x - gI.x) + abs(y - gI.y);
		}

		bool operator==(const GridIndex &gI) const {
			return x == gI.x && y == gI.y;
		}
	};
	
	int mapWidth, mapHeight;
	GridIndex goalGrid;
	std::list<GridIndex*> openList, nodesCollector;
	Grid **currentGridMap;
	uint32 **closedListLookUp;
	uint32 closedListValue;
	int gridOffsetX, gridOffsetY;

	void getAdjacentGrids(GridIndex *gridIndex);
	GridIndex *popOpenList();
	void makeGoalWalkable();
	void createPath(const GridIndex *finalGrid, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut);

public:
	PathFinderAStar(int mapWidth, int mapHeight);
	~PathFinderAStar();

	void findPath(const Vector2f *startPoint, const Vector2f *goalPoint, Grid **gridMap, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut);

	void setGridOffsets(int gridOffsetX, int gridOffsetY);
};

#endif
