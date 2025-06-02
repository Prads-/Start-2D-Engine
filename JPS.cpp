#include "JPS.h"
#include "DynamicMultidimensionalArray.h"
using namespace std;
using namespace DynamicMultidimensionalArray;

#define STRAIGHT_MOVE_COST			1.0f
#define DIAGONAL_MOVE_COST			1.414f

PathFinderJPS::PathFinderJPS(int mapWidth, int mapHeight) {
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

PathFinderJPS::~PathFinderJPS() {
	freeDynamicMultidimensionalArray(closedListLookUp, mapWidth);
}

void PathFinderJPS::findPath(const Vector2f *start, const Vector2f *goal, Grid **gridMap, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut) {
	++closedListValue;
	currentGridMap = gridMap;

	startGrid = new GridIndex();
	startGrid->g = 0.0f;
	startGrid->direction = Direction::NONE;
	GridIndexBase::positionToGridIndex(start, gridWidth, gridHeight, gridOffsetX, gridOffsetY, startGrid);
	nodesCollector.push_back(startGrid);
	openList.push_back(startGrid);

	GridIndexBase::positionToGridIndex(goal, gridWidth, gridHeight, gridOffsetX, gridOffsetY, &goalGrid);
	makeGoalWalkable();

	while(openList.size() != 0) {
		GridIndex *currentGrid = popOpenList();
		if (*currentGrid == goalGrid) {
			createPath(currentGrid, gridWidth, gridHeight, pathOut);
			break;
		}
		identifySuccessor(currentGrid);
	}

	for (GridIndex *gridIndex : nodesCollector) {
		delete gridIndex;
	}
	openList.clear();
	nodesCollector.clear();
}

void PathFinderJPS::setGridOffsets(int gridOffsetX, int gridOffsetY) {
	this->gridOffsetX = gridOffsetX;
	this->gridOffsetY = gridOffsetY;
}

PathFinderJPS::GridIndex *PathFinderJPS::popOpenList() {
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

void PathFinderJPS::getNeighbours(const GridIndex *currentGrid, std::vector<GridIndex> *neighboursOut) {
	if (currentGrid->direction == Direction::NONE) {
		getStartNeighbours(neighboursOut);
	} else {
		switch (currentGrid->direction) {
			case Direction::E:
				if (!currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y, currentGrid->g + STRAIGHT_MOVE_COST, Direction::E));
					if (currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y - 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NE));
					}
					if (currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y + 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SE));
					}
				}
				break;
			case Direction::N:
				if (!currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x, currentGrid->y - 1, currentGrid->g + STRAIGHT_MOVE_COST, Direction::N));
					if (currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y - 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NE));
					}
					if (currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y - 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NW));
					}
				}
				break;
			case Direction::NE:
				if (!currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y, currentGrid->g + STRAIGHT_MOVE_COST, Direction::E));
					if (currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y + 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SE));
					}
				}
				if (!currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x, currentGrid->y - 1, currentGrid->g + STRAIGHT_MOVE_COST, Direction::N));
					if (currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y - 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NW));
					} 
				}
				if (!currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle || !currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NE));
				}
				break;
			case Direction::NW:
				if (!currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y, currentGrid->g + STRAIGHT_MOVE_COST, Direction::W));
					if (currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y + 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SW));
					}
				}
				if (!currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x, currentGrid->y - 1, currentGrid->g + STRAIGHT_MOVE_COST, Direction::N));
					if (currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y - 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NE));
					}
				}
				if (!currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle || !currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NW));
				} 
				break;
			case Direction::S:
				if (!currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x, currentGrid->y + 1, currentGrid->g + STRAIGHT_MOVE_COST, Direction::S));
					if (currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y + 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SE));
					}
					if (currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y + 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SW));
					}
				}
				break;
			case Direction::SE:
				if (!currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y, currentGrid->g + STRAIGHT_MOVE_COST, Direction::E));
					if (currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y - 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NE));
					}
				}
				if (!currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x, currentGrid->y + 1, currentGrid->g + STRAIGHT_MOVE_COST, Direction::S));
					if (currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y + 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SW));
					}
				}
				if (!currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle || !currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SE));
				}
				break;
			case Direction::SW:
				if (!currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y, currentGrid->g + STRAIGHT_MOVE_COST, Direction::W));
					if (currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y - 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NW));
					}
				}
				if (!currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x, currentGrid->y + 1, currentGrid->g + STRAIGHT_MOVE_COST, Direction::S));
					if (currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y + 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x + 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SE));
					}
				}
				if (!currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle || !currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SW));
				}
				break;
			case Direction::W:
				if (!currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle) {
					neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y, currentGrid->g + STRAIGHT_MOVE_COST, Direction::W));
					if (currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y - 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y - 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::NW));
					}
					if (currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y + 1].isObstacle) {
						neighboursOut->push_back(GridIndex(currentGrid->x - 1, currentGrid->y + 1, currentGrid->g + DIAGONAL_MOVE_COST, Direction::SW));
					}
				}
				break;
		}
	}
}

void PathFinderJPS::getStartNeighbours(std::vector<GridIndex> *neighboursOut) {
	GridIndex neighbour;
	
	neighbour.g = STRAIGHT_MOVE_COST;	
	//West
	if (startGrid->x != 0) {
		neighbour.x = startGrid->x - 1;
		neighbour.y = startGrid->y;
		neighbour.direction = Direction::W;
		if (!currentGridMap[neighbour.x][neighbour.y].isObstacle) {
			neighboursOut->push_back(neighbour);
		}
	}
	//North
	if (startGrid->y != 0) {
		neighbour.x = startGrid->x;
		neighbour.y = startGrid->y - 1;
		neighbour.direction = Direction::N;
		if (!currentGridMap[neighbour.x][neighbour.y].isObstacle) {
			neighboursOut->push_back(neighbour);
		}
	}
	//East
	neighbour.x = startGrid->x + 1;
	neighbour.y = startGrid->y;
	neighbour.direction = Direction::E;
	if (!currentGridMap[neighbour.x][neighbour.y].isObstacle) {
		neighboursOut->push_back(neighbour);
	}
	//South
	neighbour.x = startGrid->x;
	neighbour.y = startGrid->y + 1;
	neighbour.direction = Direction::S;
	if (!currentGridMap[neighbour.x][neighbour.y].isObstacle) { 
		neighboursOut->push_back(neighbour);
	}

	neighbour.g = DIAGONAL_MOVE_COST;
	//North-West
	neighbour.x = startGrid->x - 1;
	neighbour.y = startGrid->y - 1;
	if (neighbour.x >= 0 && neighbour.y >= 0) {
		neighbour.direction = Direction::NW;
		if (!currentGridMap[neighbour.x][neighbour.y].isObstacle) {
			neighboursOut->push_back(neighbour);
		}
	}
	//South-West
	neighbour.y = startGrid->y + 1;
	if (neighbour.x >= 0) {
		neighbour.direction = Direction::SW;
		if (!currentGridMap[neighbour.x][neighbour.y].isObstacle) {
			neighboursOut->push_back(neighbour);
		}
	}
	//South-East
	neighbour.x = startGrid->x + 1;
	neighbour.direction = Direction::SE;
	if (!currentGridMap[neighbour.x][neighbour.y].isObstacle) {
		neighboursOut->push_back(neighbour);
	}
	//North-East
	neighbour.y = startGrid->y - 1;
	if (neighbour.y >= 0) {
		neighbour.direction = Direction::NE;
		if (!currentGridMap[neighbour.x][neighbour.y].isObstacle) {
			neighboursOut->push_back(neighbour);
		}
	}
}

bool PathFinderJPS::jump(GridIndex *currentGrid) {
	while (true) {
		if (*currentGrid == goalGrid) {
			return true;
		}

		if (currentGridMap[currentGrid->x][currentGrid->y].isObstacle) {
			return false;
		}

		if (currentGrid->direction == Direction::NE) {
			if (scanStraightGrids(currentGrid, Direction::N, Direction::E)) {
				return true;
			}
		} else if (currentGrid->direction == Direction::NW) {
			if (scanStraightGrids(currentGrid, Direction::N, Direction::W)) {
				return true;
			}
		} else if (currentGrid->direction == Direction::SE) {
			if (scanStraightGrids(currentGrid, Direction::S, Direction::E)) {
				return true;
			}
		} else if (currentGrid->direction == Direction::SW) {
			if (scanStraightGrids(currentGrid, Direction::S, Direction::W)) {
				return true;
			}
		}

		switch (currentGrid->direction) {
			case Direction::N:
				if ((currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y - 1].isObstacle)
					|| (currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y - 1].isObstacle)) {
						return true;
				}
				currentGrid->y -= 1;
				currentGrid->g += STRAIGHT_MOVE_COST;
				break;
			case Direction::E:
				if ((currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y - 1].isObstacle) 
					|| (currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y + 1].isObstacle)) {
						return true;
				}
				currentGrid->x += 1;
				currentGrid->g += STRAIGHT_MOVE_COST;
				break;
			case Direction::W:
				if ((currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y - 1].isObstacle) 
					|| (currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle && !currentGridMap[currentGrid->x - 1][currentGrid->y + 1].isObstacle)) {
						return true;
				}
				currentGrid->x -= 1;
				currentGrid->g += STRAIGHT_MOVE_COST;
				break;
			case Direction::S:
				if ((currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle  && !currentGridMap[currentGrid->x - 1][currentGrid->y + 1].isObstacle)
					|| (currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle && !currentGridMap[currentGrid->x + 1][currentGrid->y + 1].isObstacle)) {
						return true;
				}
				currentGrid->y += 1;
				currentGrid->g += STRAIGHT_MOVE_COST;
				break;
			case Direction::NE:
				if (currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle || currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle) {
					return true;
				}
				currentGrid->x += 1;
				currentGrid->y -= 1;
				currentGrid->g += DIAGONAL_MOVE_COST;
				break;
			case Direction::NW:
				if (currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle || currentGridMap[currentGrid->x][currentGrid->y + 1].isObstacle) {
					return true;
				}
				currentGrid->x -= 1;
				currentGrid->y -= 1;
				currentGrid->g += DIAGONAL_MOVE_COST;
				break;
			case Direction::SE:
				if (currentGridMap[currentGrid->x - 1][currentGrid->y].isObstacle || currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle) {
					return true;
				}
				currentGrid->x += 1;
				currentGrid->y += 1;
				currentGrid->g += DIAGONAL_MOVE_COST;
				break;
			case Direction::SW:
				if (currentGridMap[currentGrid->x + 1][currentGrid->y].isObstacle || currentGridMap[currentGrid->x][currentGrid->y - 1].isObstacle) {
					return true;
				}
				currentGrid->x -= 1;
				currentGrid->y += 1;
				currentGrid->g += DIAGONAL_MOVE_COST;
		}
		if (currentGridMap[currentGrid->x][currentGrid->y].isObstacle) {
			return false;
		}
	}
}

void PathFinderJPS::identifySuccessor(GridIndex *currentGrid) {
	vector<GridIndex> neighbours;
	getNeighbours(currentGrid, &neighbours);
	
	for (GridIndex &gridIndex : neighbours) {
		if (jump(&gridIndex)) {
			if (closedListLookUp[gridIndex.x][gridIndex.y] != closedListValue) {
				closedListLookUp[gridIndex.x][gridIndex.y] = closedListValue;
				GridIndex *openGrid = new GridIndex(gridIndex);
				openGrid->predecessor = currentGrid;
				openGrid->f = openGrid->g + openGrid->getManhattanDistance(goalGrid);
				nodesCollector.push_back(openGrid);
				openList.push_back(openGrid);
			}
		}
	}
}

void PathFinderJPS::makeGoalWalkable() {
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

bool PathFinderJPS::scanStraightGrids(GridIndex *currentGrid, Direction d1, Direction d2) {
	GridIndex scanStraight = *currentGrid;
	scanStraight.direction = d1;
	if (jump(&scanStraight)) {
		return true;
	}
	scanStraight = *currentGrid;
	scanStraight.direction = d2;
	if (jump(&scanStraight)) {
		return true;
	}
	return false;
}

void PathFinderJPS::createPath(const GridIndex *finalGrid, int gridWidth, int gridHeight, std::list<Vector2i> *pathOut) {
	while (finalGrid != 0) {
		Vector2i pathPoint = GridIndexBase::gridIndexToPosition(finalGrid, gridWidth, gridHeight, gridOffsetX, gridOffsetY);
		if (!currentGridMap[finalGrid->x + 1][finalGrid->y].isObstacle) {
			pathPoint.x += gridWidth / 2;
		} else if (finalGrid->x > 0 && !currentGridMap[finalGrid->x - 1][finalGrid->y].isObstacle) {
			pathPoint.x -= gridWidth / 2;
		}
		if (!currentGridMap[finalGrid->x][finalGrid->y + 1].isObstacle) {
			pathPoint.y += gridHeight / 2;
		} else if (finalGrid->y > 0 && !currentGridMap[finalGrid->x][finalGrid->y - 1].isObstacle) {
			pathPoint.y -= gridHeight / 2;
		}
		pathOut->push_front(pathPoint);
		finalGrid = finalGrid->predecessor;
	}
}