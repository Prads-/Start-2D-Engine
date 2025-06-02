#include "AStar.h"
#include "DynamicMultidimensionalArray.h"
using namespace DynamicMultidimensionalArray;
using namespace std;

PathFinderAStar::PathFinderAStar(int mapWidth, int mapHeight) {
	this->mapWidth = mapWidth;
	this->mapHeight = mapHeight;
	closedListLookUp = allocDynamicMultidimensionalArray<uint32>(mapHeight, mapWidth);
	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			closedListLookUp[x][y] = 0;
		}
	}
	closedListValue = 0;
	gridOffsetX = 0;
	gridOffsetY = 0;
}

PathFinderAStar::~PathFinderAStar() {
	freeDynamicMultidimensionalArray(closedListLookUp, mapWidth);
}

void PathFinderAStar::findPath(const Vector2f *startPoint, const Vector2f *goalPoint, Grid **gridMap, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut) {
	++closedListValue;
	currentGridMap = gridMap;

	GridIndex startIndex;
	GridIndexBase::positionToGridIndex(startPoint, gridWidth, gridHeight, gridOffsetX, gridOffsetY, &startIndex);
	startIndex.g = 0;
	startIndex.predecessor = 0;
	getAdjacentGrids(&startIndex);
	for (GridIndex *gridIndex : openList) {
		gridIndex->predecessor = 0;
	}
	GridIndexBase::positionToGridIndex(goalPoint, gridWidth, gridHeight, gridOffsetX, gridOffsetY, &goalGrid);
	goalGrid.g = 0;
	goalGrid.predecessor = 0;
	makeGoalWalkable();

	while (openList.size() != 0) {
		GridIndex *currentGrid = popOpenList();
		if (*currentGrid == goalGrid) {
			createPath(currentGrid, gridWidth, gridHeight, pathOut);
			break;
		}
		getAdjacentGrids(currentGrid);
	}

	for (GridIndex *gridIndex : nodesCollector) {
		delete gridIndex;
	}
	openList.clear();
	nodesCollector.clear();
}

void PathFinderAStar::setGridOffsets(int gridOffsetX, int gridOffsetY) {
	this->gridOffsetX = gridOffsetX;
	this->gridOffsetY = gridOffsetY;
}

void PathFinderAStar::getAdjacentGrids(GridIndex *gridIndex) {
	int g = gridIndex->g + 1;
	if (!currentGridMap[gridIndex->x + 1][gridIndex->y].isObstacle && closedListLookUp[gridIndex->x + 1][gridIndex->y] != closedListValue) {
		closedListLookUp[gridIndex->x + 1][gridIndex->y] = closedListValue;
		GridIndex *gI = new GridIndex(gridIndex->x + 1, gridIndex->y, g, gridIndex);
		gI->f = goalGrid.getManhattanDistance(*gI) + gI->g;
		openList.push_back(gI);
		nodesCollector.push_back(gI);
	}
	if (gridIndex->x != 0 && !currentGridMap[gridIndex->x - 1][gridIndex->y].isObstacle && closedListLookUp[gridIndex->x - 1][gridIndex->y] != closedListValue) {
		closedListLookUp[gridIndex->x - 1][gridIndex->y] = closedListValue;
		GridIndex *gI = new GridIndex(gridIndex->x - 1, gridIndex->y, g, gridIndex);
		gI->f = goalGrid.getManhattanDistance(*gI) + gI->g;
		openList.push_back(gI);
		nodesCollector.push_back(gI);
	}
	if (!currentGridMap[gridIndex->x][gridIndex->y + 1].isObstacle && closedListLookUp[gridIndex->x][gridIndex->y + 1] != closedListValue) {
		closedListLookUp[gridIndex->x][gridIndex->y + 1] = closedListValue;
		GridIndex *gI = new GridIndex(gridIndex->x, gridIndex->y + 1, g, gridIndex);
		gI->f = goalGrid.getManhattanDistance(*gI) + gI->g;
		openList.push_back(gI);
		nodesCollector.push_back(gI);
	}
	if (gridIndex->y != 0 && !currentGridMap[gridIndex->x][gridIndex->y - 1].isObstacle && closedListLookUp[gridIndex->x][gridIndex->y - 1] != closedListValue) {
		closedListLookUp[gridIndex->x][gridIndex->y - 1] = closedListValue;
		GridIndex *gI = new GridIndex(gridIndex->x, gridIndex->y - 1, g, gridIndex);
		gI->f = goalGrid.getManhattanDistance(*gI) + gI->g;
		openList.push_back(gI);
		nodesCollector.push_back(gI);
	}
}

PathFinderAStar::GridIndex *PathFinderAStar::popOpenList() {
	list<GridIndex*>::iterator retValIt = openList.begin(), it = ++openList.begin();
	
	while (it != openList.end()) {
		if ((*it)->f < (*retValIt)->f) {
			retValIt = it;
		}
		++it;
	}

	GridIndex *retVal = *retValIt;
	openList.erase(retValIt);

	return retVal;
}

void PathFinderAStar::makeGoalWalkable() {
	if (currentGridMap[goalGrid.x][goalGrid.y].isObstacle) {
		if (goalGrid.x > 0 && !currentGridMap[goalGrid.x - 1][goalGrid.y].isObstacle) {
			goalGrid.x -= 1;
		} else if (!currentGridMap[goalGrid.x + 1][goalGrid.y].isObstacle) {
			goalGrid.x += 1;
		} else if (goalGrid.y > 0 && !currentGridMap[goalGrid.x][goalGrid.y - 1].isObstacle) {
			goalGrid.y -= 1;
		} else if (!currentGridMap[goalGrid.x][goalGrid.y + 1].isObstacle) {
			goalGrid.y += 1;
		} else if (goalGrid.y > 0 && !currentGridMap[goalGrid.x + 1][goalGrid.y - 1].isObstacle) {
			goalGrid.x += 1;
			goalGrid.y -= 1;
		}  else if (!currentGridMap[goalGrid.x + 1][goalGrid.y + 1].isObstacle) {
			goalGrid.x += 1;
			goalGrid.y += 1;
		}  else if (goalGrid.y > 0 && goalGrid.x > 0 && !currentGridMap[goalGrid.x - 1][goalGrid.y - 1].isObstacle) {
			goalGrid.x -= 1;
			goalGrid.y -= 1;
		}  else if (goalGrid.x > 0 && !currentGridMap[goalGrid.x - 1][goalGrid.y + 1].isObstacle) {
			goalGrid.x -= 1;
			goalGrid.y += 1;
		}
	}
}

void PathFinderAStar::createPath(const GridIndex *finalGrid, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut) {
	while (finalGrid != 0) {
		pathOut->push_front(GridIndexBase::gridIndexToPosition(finalGrid, gridWidth, gridHeight, gridOffsetX, gridOffsetY));
		finalGrid = finalGrid->predecessor;
	}
}