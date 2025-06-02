#ifndef PATH_FIND_JPS_H
#define PATH_FIND_JPS_H

#include "declarations.h"
#include <vector>
#include <list>
#include <cstdlib>
#include "vector2.h"
#include "Grid.h"

enum Direction {
	NONE, N, E, W, S, NE, NW, SE, SW
};

class PathFinderJPS {
private:
	struct GridIndex : public GridIndexBase {
		float f, g;
		Direction direction;
		GridIndex *predecessor;

		GridIndex() { predecessor = 0; }

		GridIndex(int x, int y, float g, Direction direction) {
			this->x = x;
			this->y = y;
			this->direction = direction;
			this->g = g;
			predecessor = 0;
		}

		int getManhattanDistance(const GridIndex &gI) const {
			return abs(x - gI.x) + abs(y - gI.y);
		}

		bool operator ==(const GridIndex &gI) const {
			return gI.x == x && gI.y == y;
		}
	};

	int mapWidth, mapHeight;
	GridIndex *startGrid, goalGrid;
	std::list<GridIndex*> openList, nodesCollector;
	Grid **currentGridMap;
	uint32 **closedListLookUp;
	uint32 closedListValue;
	int gridOffsetX, gridOffsetY;

	GridIndex *popOpenList();
	void getNeighbours(const GridIndex *currentGrid, std::vector<GridIndex> *neighboursOut);
	void getStartNeighbours(std::vector<GridIndex> *neighboursOut);
	bool jump(GridIndex *currentGrid);
	void identifySuccessor(GridIndex *currentGrid);
	void makeGoalWalkable();
	bool scanStraightGrids(GridIndex *currentGrid, Direction d1, Direction d2);
	void createPath(const GridIndex *finalGrid, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut);

public:
	PathFinderJPS(int mapWidth, int mapHeight);
	~PathFinderJPS();

	void findPath(const Vector2f *start, const Vector2f *goal, Grid **gridMap, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut);

	void setGridOffsets(int gridOffsetX, int gridOffsetY);
};

#endif