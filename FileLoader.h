#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <string>
#include <vector>
#include <list>
#include "BoundingBox.h"
#include "Character.h"
#include "PickupEnums.h"

namespace FileLoader {
	struct LevelAsset {
		std::string path;
		
		struct Clone {
			int rotationAngle;
			bool flipX, flipY, alphaBlend, isPrioritised;
			int x, y;

			Clone() {
				rotationAngle = 0;
				flipX = false;
				flipY = false;
				alphaBlend = false;
				isPrioritised = false;
				x = 0;
				y = 0;
			}
		};
		std::vector<Clone> clones;
	};

	struct RawEnemy {
		int id, x, y;
	};

	struct RawCampaignPickup {
		int x, y, interval, usageCount, respawnCount;
		std::vector<PickupMove> moves;
	};

	struct RawEnemyWave {
		std::vector<RawEnemy> enemies;
		std::vector<RawCampaignPickup> pickups;
		bool isShipWave;
		int shipId, shipX, shipY;
	};

	struct RawLevel {
		int mapId;
		Vector2f playerPosition;
		Vector2i gridOffset;
		std::list<RawEnemyWave> waves;

		RawLevel() {
			mapId = 0;
			playerPosition = Vector2f(512.0f, 384.0f);
			gridOffset = Vector2i(0, 0);
		}
	};

	void loadLevelAssetsFile(std::string path, std::vector<LevelAsset> *assetsOut);
	void loadLevelBoundingBox(std::string path, std::vector<BoundingBox> *bbOut);
	void loadCharacterParameters(std::string path, Character *characterOut);
	void loadLevelData(std::string path, RawLevel *rawLevelOut);
	
	void loadMapList(std::vector<std::string> *mapListOut);
	void loadCharacterList(std::vector<std::string> *characterListOut);

	void loadSurvivalThemeList(std::vector<std::string> *themeListOut);
}

#endif