#include "RandomLevelGenerator.h"
#include "Grid.h"
#include "StartEngine.h"
#include "MT_RandomGenerator.h"
#include "EnemiesId.h"
#include "FileLoader.h"
#include "stringExtension.h"
using namespace std;
using namespace FileLoader;
using namespace StringExtension;

#define CORRIDOR_SIZE		80
#define BORDER_LINE_WIDTH	4

RandomLevelGenerator::RandomLevelGenerator(StartEngine *engine, std::vector<BoundingBox> *bBoxes) {
	this->bBoxes = bBoxes;
	pickupMoves.push_back(PICKUP_BATTERY);
	pickupMoves.push_back(PICKUP_CALL_BALL);
	pickupMoves.push_back(PICKUP_GRANADE);
	pickupMoves.push_back(PICKUP_HOMING);
	pickupMoves.push_back(PICKUP_IMMOBILISER);
	pickupMoves.push_back(PICKUP_PIERCING_BALL);
	pickupMoves.push_back(PICKUP_PINBALL);
	pickupMoves.push_back(PICKUP_SHIELD);
	generateRandomLevel(engine);
}

std::vector<int> RandomLevelGenerator::getEnemies() {
	return enemies;
}

const FileLoader::RawCampaignPickup *RandomLevelGenerator::getPickupItem() const {
	return &pickupItem;
}

void RandomLevelGenerator::generateRandomLevel(StartEngine *engine) {
	Grid grids[(FRAME_WIDTH / CORRIDOR_SIZE) * (FRAME_HEIGHT / CORRIDOR_SIZE)];
	for (int i = 0; i < (FRAME_WIDTH / CORRIDOR_SIZE) * (FRAME_HEIGHT / CORRIDOR_SIZE); ++i) {
		grids[i].isObstacle = true;
	}

	carveOutCorridors(grids);
	generateBoundingBoxes(grids);
	createBackgroundImage(engine, grids);
	createEnemies();
	createPickupItem();
}

void RandomLevelGenerator::carveOutCorridors(Grid *grids) {
	int numberOfCorridors = 10 + (MT_RandomGenerator::randomNumber() % 6);
	
	Vector2i p1;
	p1.x = 1 + MT_RandomGenerator::randomNumber() % (FRAME_WIDTH / CORRIDOR_SIZE - 1);
	p1.y = 1 + MT_RandomGenerator::randomNumber() % (FRAME_HEIGHT / CORRIDOR_SIZE - 1);
	
	for (int i = 0; i < numberOfCorridors; ++i) {
		Vector2i p2;
		p2.x = 1 + MT_RandomGenerator::randomNumber() % (FRAME_WIDTH / CORRIDOR_SIZE - 1);
		p2.y = 1 + MT_RandomGenerator::randomNumber() % (FRAME_HEIGHT / CORRIDOR_SIZE - 1);

		int addValue = (p1.x < p2.x) ? 1 : -1;
		grids[p1.x + p1.y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle = false;
		while (p1.x != p2.x) {
			p1.x += addValue;
			grids[p1.x + p1.y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle = false;
		}

		addValue = (p1.y < p2.y) ? 1 : -1;
		while (p1.y != p2.y) {
			p1.y += addValue;
			grids[p1.x + p1.y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle = false;
		}
		
		p1 = p2;
	}
}

void RandomLevelGenerator::generateBoundingBoxes(Grid *grids) {
	(*bBoxes)[1].minP.y = 720.0f;
	(*bBoxes)[3].minP.x = 960.0f;

	bool checkedList[(FRAME_WIDTH / CORRIDOR_SIZE) * (FRAME_HEIGHT / CORRIDOR_SIZE)];
	for (int i = 0; i < (FRAME_WIDTH / CORRIDOR_SIZE) * (FRAME_HEIGHT / CORRIDOR_SIZE); ++i) {
		checkedList[i] = false;
	}

	Vector2i uncheckedIndex;
	while (getFirstUncheckedIndex(checkedList, &uncheckedIndex)) {
		if (grids[uncheckedIndex.x + uncheckedIndex.y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
			BoundingBox bBox = generateBoundingBox(grids, checkedList, uncheckedIndex);
			bBoxes->push_back(bBox);
		} else {
			checkedList[uncheckedIndex.x + uncheckedIndex.y * (FRAME_WIDTH / CORRIDOR_SIZE)] = true;
		}
	}
}

bool RandomLevelGenerator::getFirstUncheckedIndex(bool *checkedList, Vector2i *indexOut) {
	for (int y = 0; y < FRAME_HEIGHT / CORRIDOR_SIZE; ++y) {
		for (int x = 0; x < FRAME_WIDTH / CORRIDOR_SIZE; ++x) {
			if (!checkedList[x + y * (FRAME_WIDTH / CORRIDOR_SIZE)]) {
				indexOut->x = x;
				indexOut->y = y;
				return true;
			}
		}
	}
	return false;
}

BoundingBox RandomLevelGenerator::generateBoundingBox(Grid *grids, bool *checkedList, Vector2i startIndex) {
	BoundingBox retVal;
	retVal.minP = Vector2f(startIndex.x * CORRIDOR_SIZE, startIndex.y * CORRIDOR_SIZE);
	const Vector2i START_INDEX = startIndex;
	checkedList[startIndex.x + startIndex.y * (FRAME_WIDTH / CORRIDOR_SIZE)] = true;

	for (int i = startIndex.x + 1; i < FRAME_WIDTH / CORRIDOR_SIZE; ++i) {
		if (grids[i + startIndex.y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
			startIndex.x = i;
		} else {
			break;
		}
		checkedList[startIndex.x + startIndex.y * (FRAME_WIDTH / CORRIDOR_SIZE)] = true;
	}

	for (int i = startIndex.y + 1; i < FRAME_HEIGHT / CORRIDOR_SIZE; ++i) {
		if (checkIfRowIsAllObstacle(grids, START_INDEX.x, startIndex.x, i)) {
			startIndex.y = i;
			for (int i = START_INDEX.x; i <= startIndex.x; ++i) {
				checkedList[i + startIndex.y * (FRAME_WIDTH / CORRIDOR_SIZE)] = true;
			}
		} else {
			break;
		}
	}

	retVal.maxP = Vector2f(startIndex.x * CORRIDOR_SIZE + CORRIDOR_SIZE, startIndex.y * CORRIDOR_SIZE + CORRIDOR_SIZE);

	//Hack
	if (retVal.maxP.x == 960.0f) {
		retVal.maxP.x = 1023.0f;
	}
	if (retVal.maxP.y == 720.0f) {
		retVal.maxP.y = 767.0f;
	}

	return retVal;
}

bool RandomLevelGenerator::checkIfRowIsAllObstacle(Grid *grids, int startX, int endX, int y) {
	while (startX <= endX) {
		if (!grids[startX + y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
			return false;
		}
		++startX;
	}
	return true;
}

void RandomLevelGenerator::createBackgroundImage(StartEngine *engine, Grid *grids) {
	vector<Image*> wallTiles, floorTiles, wallTilesRatio, floorTilesRatio;
	loadWallAndFloorTiles(&wallTiles, &floorTiles, &wallTilesRatio, &floorTilesRatio);
	
	engine->clearBackground(0xFFFFFF);
	uint32 *backgroundBuffer = engine->getBackgroundBuffer();

	for (int y = 0; y < FRAME_HEIGHT; y += CORRIDOR_SIZE) {
		for (int x = 0; x < FRAME_WIDTH; x += CORRIDOR_SIZE) {
			int wallTileIndex = MT_RandomGenerator::randomNumber() % wallTilesRatio.size();
			engine->drawBackground(wallTilesRatio[wallTileIndex], 0, MT_RandomGenerator::randomNumber() % 2, MT_RandomGenerator::randomNumber() % 2, false, x, y);
		}
	}

	for (int y = 0; y < FRAME_HEIGHT / CORRIDOR_SIZE; ++y) {
		for (int x = 0; x < FRAME_WIDTH / CORRIDOR_SIZE; ++x) {
			if (grids[x + y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
				if (x != 0) { 
					if (!grids[x - 1 + y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
						drawVerticalBorderLine(Vector2i(x * CORRIDOR_SIZE, y * CORRIDOR_SIZE), Vector2i(x * CORRIDOR_SIZE, y * CORRIDOR_SIZE + CORRIDOR_SIZE), backgroundBuffer);
					}
				}
				if (y != 0 && !grids[x + (y - 1) * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
					drawHorizontalBorderLine(Vector2i(x * CORRIDOR_SIZE, y * CORRIDOR_SIZE), Vector2i(x * CORRIDOR_SIZE + CORRIDOR_SIZE, y * CORRIDOR_SIZE), backgroundBuffer);
				}
				if (x != (FRAME_WIDTH / CORRIDOR_SIZE) - 1 && !grids[x + 1 + y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
					drawVerticalBorderLine(Vector2i(x * CORRIDOR_SIZE + CORRIDOR_SIZE - BORDER_LINE_WIDTH, y * CORRIDOR_SIZE), Vector2i(x * CORRIDOR_SIZE + CORRIDOR_SIZE - BORDER_LINE_WIDTH, y * CORRIDOR_SIZE + CORRIDOR_SIZE), backgroundBuffer);
				}
				if (y != (FRAME_HEIGHT / CORRIDOR_SIZE) - 1 && !grids[x + (
					y + 1) * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
					drawHorizontalBorderLine(Vector2i(x * CORRIDOR_SIZE, y * CORRIDOR_SIZE + CORRIDOR_SIZE - BORDER_LINE_WIDTH), Vector2i(x * CORRIDOR_SIZE + CORRIDOR_SIZE, y * CORRIDOR_SIZE + CORRIDOR_SIZE - BORDER_LINE_WIDTH), backgroundBuffer);
				}
			} else {
				int floorTileIndex = MT_RandomGenerator::randomNumber() % floorTilesRatio.size();
				engine->drawBackground(floorTilesRatio[floorTileIndex], 0, MT_RandomGenerator::randomNumber() % 2, MT_RandomGenerator::randomNumber() % 2, false, x * CORRIDOR_SIZE, y * CORRIDOR_SIZE);
				if (x != 0 && y != 0 && grids[x - 1 + y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle && grids[x + (y - 1) * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
					drawVerticalBorderLine(Vector2i(x * CORRIDOR_SIZE - BORDER_LINE_WIDTH, y * CORRIDOR_SIZE - BORDER_LINE_WIDTH), Vector2i(x * CORRIDOR_SIZE - BORDER_LINE_WIDTH, y * CORRIDOR_SIZE), backgroundBuffer);
				}
				if (x != 0 && y != (FRAME_HEIGHT / CORRIDOR_SIZE) - 1 && grids[x - 1 + y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle && grids[x + (y + 1) * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
					drawVerticalBorderLine(Vector2i(x * CORRIDOR_SIZE - BORDER_LINE_WIDTH, y * CORRIDOR_SIZE + CORRIDOR_SIZE), Vector2i(x * CORRIDOR_SIZE - BORDER_LINE_WIDTH, y * CORRIDOR_SIZE + CORRIDOR_SIZE + BORDER_LINE_WIDTH), backgroundBuffer);
				}
				if (x != (FRAME_WIDTH / CORRIDOR_SIZE) - 1 && y != 0 && grids[x + 1 + y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle && grids[x + (y - 1) * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
					drawVerticalBorderLine(Vector2i(x * CORRIDOR_SIZE + CORRIDOR_SIZE, y * CORRIDOR_SIZE - BORDER_LINE_WIDTH), Vector2i(x * CORRIDOR_SIZE + CORRIDOR_SIZE, y * CORRIDOR_SIZE), backgroundBuffer);
				}
				if (x != (FRAME_WIDTH / CORRIDOR_SIZE) - 1 && y != (FRAME_HEIGHT / CORRIDOR_SIZE) - 1 && grids[x + 1 + y * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle && grids[x + (y + 1) * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
					drawVerticalBorderLine(Vector2i(x * CORRIDOR_SIZE + CORRIDOR_SIZE, y * CORRIDOR_SIZE + CORRIDOR_SIZE), Vector2i(x * CORRIDOR_SIZE + CORRIDOR_SIZE, y * CORRIDOR_SIZE + CORRIDOR_SIZE + BORDER_LINE_WIDTH), backgroundBuffer);
				}
			}
		}
	}

	//Edges
	for (int i = 0; i < FRAME_HEIGHT / CORRIDOR_SIZE; ++i) {
		if (!grids[FRAME_WIDTH / CORRIDOR_SIZE - 1 + i * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
			drawVerticalBorderLine(Vector2i(960.0f, i * CORRIDOR_SIZE - BORDER_LINE_WIDTH), Vector2i(960.0f, i * CORRIDOR_SIZE + CORRIDOR_SIZE + BORDER_LINE_WIDTH), backgroundBuffer);
		}
	}
	for (int i = 0; i < FRAME_WIDTH / CORRIDOR_SIZE; ++i) {
		if (!grids[i + (FRAME_HEIGHT / CORRIDOR_SIZE - 1) * (FRAME_WIDTH / CORRIDOR_SIZE)].isObstacle) {
			drawHorizontalBorderLine(Vector2i(i * CORRIDOR_SIZE - BORDER_LINE_WIDTH, 720.0f), Vector2i(i * CORRIDOR_SIZE + CORRIDOR_SIZE + BORDER_LINE_WIDTH, 720.0f), backgroundBuffer);
		}
	}

	for (Image *tile : wallTiles) {
		delete tile;
	}
	for (Image *tile :floorTiles) {
		delete tile;
	}
}

void RandomLevelGenerator::loadWallAndFloorTiles(std::vector<Image*> *wallTilesOut, std::vector<Image*> *floorTilesOut, std::vector<Image*> *wallTilesRatioOut, std::vector<Image*> *floorTilesRatioOut) {
	vector<string> themeList;
	loadSurvivalThemeList(&themeList);

	string themePath = themeList[MT_RandomGenerator::randomNumber() % themeList.size()];
	themePath = "Images/SurvivalThemes/" + themePath;
	
	int tileCounter = 1;
	while (true) {
		Image *wallTile = new Image();
		if (wallTile->loadImage(themePath + "/Wall" + intToStr(tileCounter) + ".png")) {
			wallTilesOut->push_back(wallTile);
		} else {
			delete wallTile;
			break;
		}
		++tileCounter;
	}

	tileCounter = 1;
	while (true) {
		Image *floorTile = new Image();
		if (floorTile->loadImage(themePath + "/Floor" + intToStr(tileCounter) + ".png")) {
			floorTilesOut->push_back(floorTile);
		} else {
			delete floorTile;
			break;
		}
		++tileCounter;
	}

	loadRatioFile(themePath + "/floor.ratio", floorTilesOut, floorTilesRatioOut);
	loadRatioFile(themePath + "/wall.ratio", wallTilesOut, wallTilesRatioOut);
}

void RandomLevelGenerator::loadRatioFile(std::string path, const std::vector<Image*> *tiles, std::vector<Image*> *tilesRatio) {
	for (Image *tile : *tiles) {
		tilesRatio->push_back(tile);
	}
	
	ifstream wallTilesRatioFile(path);
	if (wallTilesRatioFile.is_open()) {
		for (Image *tile : *tiles) {
			int ratioValue;
			if (wallTilesRatioFile >> ratioValue) {
				wallTilesRatioFile.get();
				for (int i = 0; i < ratioValue; ++i) {
					tilesRatio->push_back(tile);
				}
			} else {
				break;
			}
		}
	}
}

void RandomLevelGenerator::drawVerticalBorderLine(Vector2i from, Vector2i to, uint32 *backgroundBuffer) {
	while (from.y < to.y) {
		for (int i = 0; i < BORDER_LINE_WIDTH; ++i) {
			backgroundBuffer[from.x + i + from.y * FRAME_WIDTH] = 1;
		}
		++from.y;
	}
}

void RandomLevelGenerator::drawHorizontalBorderLine(Vector2i from, Vector2i to, uint32 *backgroundBuffer) {
	while (from.x < to.x) {
		for (int i = 0; i < BORDER_LINE_WIDTH; ++i) {
			backgroundBuffer[from.x + ((from.y + i) * FRAME_WIDTH)] = 1;
		}
		++from.x;
	}
}

void RandomLevelGenerator::createEnemies() {
	float weight = 0.0f;
	while (weight < 5.0f) {
		int enemyId = MT_RandomGenerator::randomNumber() % NUMBER_OF_ENEMIES;
		if (enemyId == ENEMY_ID_MB01GOLEM) {
			continue;
		}
		enemies.push_back(enemyId);
		
		//VC++ compiler bug generates wrong code if I call getEnemyWeightById: https://connect.microsoft.com/VisualStudio/feedback/details/768562/visual-c-11-produces-bad-assembly-code-when-using-whole-program-optimization-for-x86-target#tabs
		//weight += getEnemyWeightById(enemyId);
		switch (enemyId) {
			case ENEMY_ID_DRAGOON:
				weight += 1.8f;
				break;
			case ENEMY_ID_GUARDIAN:
				weight += 1.8f;
				break;
			case ENEMY_ID_MINION1:
				weight += 0.5f;
				break;
			case ENEMY_ID_MINION2:
				weight += 1.0f;
				break;
			case ENEMY_ID_MINION_EGG:
				weight += 2.0f;
				break;
			case ENEMY_ID_PATROLLER:
				weight += 1.7f;
				break;
			case ENEMY_ID_PROXIMITY_DRONE:
				weight += 1.2f;
				break;
			case ENEMY_ID_SENTRY:
				weight += 1.5f;
				break;
			case ENEMY_ID_SWEEPER:
				weight += 1.0f;
				break;
			case ENEMY_ID_E01SENSOR:
				weight += 2.0f;
				break;
			case ENEMY_ID_E02ROVER:
				weight += 1.3f;
				break;
			case ENEMY_ID_SPIKE_SHIP:
				weight += 1.7f;
				break;
			case ENEMY_ID_THE_EYE:
				weight += 1.5f;
				break;
			case ENEMY_ID_MB01GOLEM:
				weight += 4.5f;
		}
	}
}

float RandomLevelGenerator::getEnemyWeightById(int enemyId) {
	//Since this code cannot be used due to VC++ compiler bug, it is not maintained to match the above 'weight by id' swtch code.
	//If it can be used in future, make sure you update the return values to match the above code...
	/*switch (enemyId) {
		case ENEMY_ID_DRAGOON:
			return 1.8f;
		case ENEMY_ID_GUARDIAN:
			return 1.8f;
		case ENEMY_ID_MINION1:
			return 0.5f;
		case ENEMY_ID_MINION2:
			return 1.0f;
		case ENEMY_ID_MINION_EGG:
			return 2.3f;
		case ENEMY_ID_PATROLLER:
			return 1.7f;
		case ENEMY_ID_PROXIMITY_DRONE:
			return 0.8f;
		case ENEMY_ID_SENTRY:
			return 1.7f;
		case ENEMY_ID_SWEEPER:
			return 1.0f;
		case ENEMY_ID_E01SENSOR:
			return 2.3f;
		case ENEMY_ID_E02ROVER:
			return 1.3f;
		case ENEMY_ID_SPIKE_SHIP:
			return 1.7f;
		case ENEMY_ID_THE_EYE:
			return 1.8f;
		case ENEMY_ID_MB01GOLEM:
			return 4.5f;
	}*/
	return 0.0f;
}

void RandomLevelGenerator::createPickupItem() {
	pickupItem.interval = 300 + MT_RandomGenerator::randomNumber() % 300;
	pickupItem.respawnCount = 1 + MT_RandomGenerator::randomNumber() % 10;
	pickupItem.usageCount =	1 + MT_RandomGenerator::randomNumber() % 3;
	int numberOfMoves = 1 + MT_RandomGenerator::randomNumber() % (pickupMoves.size() - 1);
	for (int i = 0; i < numberOfMoves; ++i) {
		pickupItem.moves.push_back(pickupMoves[MT_RandomGenerator::randomNumber() % pickupMoves.size()]);
	}
}