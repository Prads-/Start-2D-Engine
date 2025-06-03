#ifndef RANDOM_LEVEL_GENERATOR_H
#define RANDOM_LEVEL_GENERATOR_H

#include "BoundingBox.h"
#include <vector>
#include "Image.h"
#include "Grid.h"
#include "FileLoader.h"
#include "PickupEnums.h"
#include "StartEngine.h"
#include "EnemiesId.h"

class RandomLevelGenerator {
private:
	std::vector<BoundingBox> *bBoxes;
	std::vector<int> enemies;
	FileLoader::RawCampaignPickup pickupItem;
	std::vector<PickupMove> pickupMoves;

	void generateRandomLevel(StartEngine *engine);
	void carveOutCorridors(Grid *grids);
	void generateBoundingBoxes(Grid *grids);
	bool getFirstUncheckedIndex(bool *checkedList, Vector2i *indexOut);
	BoundingBox generateBoundingBox(Grid *grids, bool *checkedList, Vector2i startIndex);
	bool checkIfRowIsAllObstacle(Grid *grids, int startX, int endX, int y);
	void createBackgroundImage(StartEngine *engine, Grid *grids);
	void createEnemies();
	float getEnemyWeightById(int enemyId);
	void createPickupItem();
	void drawVerticalBorderLine(Vector2i from, Vector2i to, uint32 *backgroundBuffer);
	void drawHorizontalBorderLine(Vector2i from, Vector2i to, uint32 *backgroundBuffer);
	void loadWallAndFloorTiles(std::vector<Image*> *wallTilesOut, std::vector<Image*> *floorTilesOut, std::vector<Image*> *wallTilesRatioOut, std::vector<Image*> *floorTilesRatioOut);
	void loadRatioFile(std::string path, const std::vector<Image*> *tiles, std::vector<Image*> *tilesRatio);

public:
	RandomLevelGenerator(StartEngine *engine, std::vector<BoundingBox> *bBoxes);

	std::vector<int> getEnemies();
	const FileLoader::RawCampaignPickup *getPickupItem() const;
};

#endif