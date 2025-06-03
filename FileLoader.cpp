#include "FileLoader.h"
#include <fstream>
#include "FileLoaderException.h"
#include "FileParser.h"
#include "stringExtension.h"
#include <stdexcept>
#include "Character.h"
using namespace std;
using namespace FileParser;
using namespace StringExtension;

static bool getFileLines(string path, vector<string> *linesOut);

static bool createAssetList(const vector<Token*> *tokens, vector<FileLoader::LevelAsset> *assetsOut);
static bool createAssetStructure(const vector<Token*> *tokens, int &counter, vector<FileLoader::LevelAsset> *assetsOut);
static bool createAssetClone(const vector<Token*> *tokens, int &counter, FileLoader::LevelAsset *levelAsset);

static bool createBBList(const vector<Token*> *tokens, vector<BoundingBox> *bbOut);
static bool createBBStructure(const vector<Token*> *tokens, int &counter, vector<BoundingBox> *bbOut);

static bool createCharacter(const vector<Token*> *tokens, Character *character);

static bool createRawLevel(const vector<Token*> *tokens, FileLoader::RawLevel *rawLevelOut); 
static bool createRawWave(const vector<Token*> *tokens, int &counter, FileLoader::RawEnemyWave *rawWaveOut);
static bool createRawEnemy(const vector<Token*> *tokens, int &counter, FileLoader::RawEnemy *rawEnemyOut);
static bool createRawCampaignPickup(const vector<Token*> *tokens, int &counter, FileLoader::RawCampaignPickup *rawCampaignPickupOut);
static bool createPickupMoves(const vector<Token*> *tokens, int &counter, vector<PickupMove> *pickupMovesOut);
static bool createPlayerPosition(const vector<Token*> *tokens, int &counter, Vector2f *playerPositionOut);
static bool createGridOffset(const vector<Token*> *tokens, int &counter, Vector2i *gridOffsetOut);

static void loadListFile(string listPath, vector<string> *listOut);

static bool getFileLines(string path, vector<string> *linesOut) {
	ifstream file(path);
	if (file.is_open()) {
		string line;
		
		while (getLineFromFile(&file, &line)) {
			linesOut->push_back(line);
		}

		file.close();
		return true;
	} else {
		return false;
	}
}

void FileLoader::loadLevelAssetsFile(std::string path, std::vector<LevelAsset> *assetsOut) {
	bool success = false;
	vector<string> srcLines;
	
	if (getFileLines(path, &srcLines)) {
		vector<Token*> tokens = parseEnvironmentFileString(&srcLines);
		
		success = createAssetList(&tokens, assetsOut);

		for (Token *token : tokens) {
			delete token;
		}
	} else {
		throw FileLoaderException("Could not open assets file " + path);
	}

	if (!success) {
		throw FileLoaderException("Asset file " + path + " seems to be corrupted");
	}
}

void FileLoader::loadLevelBoundingBox(std::string path, std::vector<BoundingBox> *bbOut) {
	bool success;
	vector<string> srcLines;
		
	if (getFileLines(path, &srcLines)) {
		vector<Token*> tokens = parseEnvironmentFileString(&srcLines);

		success = createBBList(&tokens, bbOut);
		
		for (Token *token : tokens) {
			delete token;
		}
	} else {
		throw FileLoaderException("Could not open bounding box file " + path);
	}

	if (!success) {
		throw FileLoaderException("Bounding box " + path + " file seems to be corrupted");
	}
}

void FileLoader::loadCharacterParameters(std::string path, Character *characterOut) {
	vector<string> lines;
	bool success;

	if (getFileLines(path, &lines)) {
		vector<Token*> tokens = parseCharacterFileString(&lines);

		success = createCharacter(&tokens, characterOut);

		for (Token *token : tokens) {
			delete token;
		}
	} else {
		throw FileLoaderException("Could not open character parameter file" + path);
	}

	if (!success) {
		throw FileLoaderException("Character parameter " + path + " file seems to be corrupted");
	}
}

void FileLoader::loadLevelData(std::string path, RawLevel *rawLevelOut) {
	vector<string> lines;
	bool success;

	if (getFileLines(path, &lines)) {
		vector<Token*> tokens = parseLevelFileString(&lines);

		success = createRawLevel(&tokens, rawLevelOut);

		for (Token *token : tokens) {
			delete token;
		}
	} else {
		throw FileLoaderException("Could not open level file" + path);
	}

	if (!success) {
		throw FileLoaderException("Level " + path + " file seems to be corrupted");
	}
}

static void loadListFile(string listPath, vector<string> *listOut) {
	ifstream listFile(listPath);
	if (listFile.is_open()) {
		string line;
		while (getLineFromFile(&listFile, &line)) {
			listOut->push_back(line);
		}
	}
}

void FileLoader::loadMapList(std::vector<std::string> *mapListOut) {
	loadListFile("maps/list.txt", mapListOut);
}

void FileLoader::loadCharacterList(std::vector<std::string> *characterListOut) {
	loadListFile("characters/list.txt", characterListOut);
}

void FileLoader::loadSurvivalThemeList(std::vector<std::string> *themeListOut) {
	loadListFile("Images/SurvivalThemes/list.txt", themeListOut);
}

static bool createAssetList(const vector<Token*> *tokens, vector<FileLoader::LevelAsset> *assetsOut) {
	for (int counter = 0; counter < tokens->size(); ++counter) {
		if ((*tokens)[counter]->getKeyword() == KW_START_ASSET) {
			if (!createAssetStructure(tokens, ++counter, assetsOut)) {
				return false;
			}
		} else {
			return false;
		}
	}

	return true;
}

static bool createAssetStructure(const vector<Token*> *tokens, int &counter, vector<FileLoader::LevelAsset> *assetsOut) {
	FileLoader::LevelAsset levelAsset;

	try {
		for (; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_PATH:
					++counter;
					levelAsset.path = tokens->at(counter)->getStringValue();
					break;
				case KW_START_CLONE:
					if (!createAssetClone(tokens, ++counter, &levelAsset)) {
						return false;
					}
					break;
				case KW_CLOSE_ASSET:
					assetsOut->push_back(levelAsset);
					return true;
				default:
					return false;
			}
		}
	} catch (out_of_range) { }

	return false;
}

static bool createAssetClone(const vector<Token*> *tokens, int &counter, FileLoader::LevelAsset *levelAsset) {
	FileLoader::LevelAsset::Clone clone;

	try {
		for (; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_ROTATION_ANGLE:
					clone.rotationAngle = tokens->at(++counter)->getIntValue();
					break;
				case KW_FLIP_X:
					clone.flipX = tokens->at(++counter)->getBoolValue();
					break;
				case KW_FLIP_Y:
					clone.flipY = tokens->at(++counter)->getBoolValue();
					break;
				case KW_ALPHA_BLEND:
					clone.alphaBlend = tokens->at(++counter)->getBoolValue();
					break;
				case KW_X:
					clone.x = tokens->at(++counter)->getIntValue();
					break;
				case KW_Y:
					clone.y = tokens->at(++counter)->getIntValue();
					break;
				case KW_IS_PRIORITISED:
					clone.isPrioritised = tokens->at(++counter)->getBoolValue();
					break;
				case KW_CLOSE_CLONE:
					levelAsset->clones.push_back(clone);
					return true;
				default:
					return false;
			}
		}
	} catch (out_of_range) { }

	return false;
}

static bool createBBList(const vector<Token*> *tokens, vector<BoundingBox> *bbOut) {
	for (int counter = 0; counter < tokens->size(); ++counter) {
		if ((*tokens)[counter]->getKeyword() == KW_START_BB) {
			if (!createBBStructure(tokens, ++counter, bbOut)) {
				return false;
			}
		} else {
			return false;
		}
	}

	return true;
}

static bool createBBStructure(const vector<Token*> *tokens, int &counter, vector<BoundingBox> *bbOut) {
	BoundingBox bBox;

	try {
		for (; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_MIN_X:
					bBox.minP.x = tokens->at(++counter)->getIntValue();
					break;
				case KW_MIN_Y:
					bBox.minP.y = tokens->at(++counter)->getIntValue();
					break;
				case KW_MAX_X:
					bBox.maxP.x = tokens->at(++counter)->getIntValue();
					break;
				case KW_MAX_Y:
					bBox.maxP.y = tokens->at(++counter)->getIntValue();
					break;
				case KW_CLOSE_BB:
					bbOut->push_back(bBox);
					return true;
				default:
					return false;
			}
		}
	} catch (out_of_range) { }

	return false;
}

static bool createCharacter(const vector<Token*> *tokens, Character *character) {
	AIVariables aiVariables;

	try {
		for (int counter = 0; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_NAME:
					character->setName(tokens->at(++counter)->getStringValue());
					break;
				case KW_WIDTH:
					aiVariables.width = tokens->at(++counter)->getIntValue();
					break;
				case KW_HEIGHT:
					aiVariables.height = tokens->at(++counter)->getIntValue();
					break;
				case KW_MAX_SPEED:
					aiVariables.maxSpeed = tokens->at(++counter)->getFloatValue();
					break;
				case KW_CAUTION_DISTANCE:
					aiVariables.cautionDistance = tokens->at(++counter)->getFloatValue();
					break;
				case KW_SHOOTING_SHARPNESS:
					aiVariables.shootingSharpness = tokens->at(++counter)->getFloatValue();
					break;
				case KW_SPECIAL_THROW:
					aiVariables.specialThrow = tokens->at(++counter)->getIntValue();
					break;
				case KW_SECONDARY_ABILITY:
					aiVariables.secondaryAbility = tokens->at(++counter)->getIntValue();
					break;
			}
		}
	} catch (out_of_range) { 
		return false; 
	}

	character->setAIVariables(aiVariables);
	return true;
}

static bool createRawLevel(const vector<Token*> *tokens, FileLoader::RawLevel *rawLevelOut) {
	try {
		for (int counter = 0; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_ID:
					rawLevelOut->mapId = tokens->at(++counter)->getIntValue();
					break;
				case KW_START_WAVE:
					{
						FileLoader::RawEnemyWave rawEnemyWave;
						if (!createRawWave(tokens, ++counter, &rawEnemyWave)) {
							return false;
						}
						rawLevelOut->waves.push_back(rawEnemyWave);
					}
					break;
				case KW_START_PLAYER_POSITION:
					if (!createPlayerPosition(tokens, ++counter, &rawLevelOut->playerPosition)) {
						return false;
					}
					break;
				case KW_START_GRID_OFFSET:
					if (!createGridOffset(tokens, ++counter, &rawLevelOut->gridOffset)) {
						return false;
					}
					break;
				default:
					return false;
			}
		}
	} catch (out_of_range) {
		return false;
	}
	return true;
}

static bool createRawWave(const vector<Token*> *tokens, int &counter, FileLoader::RawEnemyWave *rawWaveOut) {
	for (; counter < tokens->size(); ++counter) {
		switch ((*tokens)[counter]->getKeyword()) {
			case KW_START_ENEMY:
				{
					FileLoader::RawEnemy rawEnemy;
					if (!createRawEnemy(tokens, ++counter, &rawEnemy)) {
						return false;
					}
					rawWaveOut->enemies.push_back(rawEnemy);
				}
				break;
			case KW_START_SHIP:
				{
					FileLoader::RawEnemy rawEnemyShip;
					if (!createRawEnemy(tokens, ++counter, &rawEnemyShip)) {
						return false;
					}
					rawWaveOut->isShipWave = true;
					rawWaveOut->shipId = rawEnemyShip.id;
					rawWaveOut->shipX = rawEnemyShip.x;
					rawWaveOut->shipY = rawEnemyShip.y;
				}
				break;
			case KW_START_PICKUP:
					{
						FileLoader::RawCampaignPickup rawCampaignPickup;
						if (!createRawCampaignPickup(tokens, ++counter, &rawCampaignPickup)) {
							return false;
						}
						rawWaveOut->pickups.push_back(rawCampaignPickup);
					}
					break;
			case KW_CLOSE_WAVE:
				return true;
				break;
			default:
				return false;
		}
	}
	return false;
}

static bool createRawEnemy(const vector<Token*> *tokens, int &counter, FileLoader::RawEnemy *rawEnemyOut) {
	try {
		for (; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_ID:
					rawEnemyOut->id = tokens->at(++counter)->getIntValue();
					break;
				case KW_X:
					rawEnemyOut->x = tokens->at(++counter)->getIntValue();
					break;
				case KW_Y:
					rawEnemyOut->y = tokens->at(++counter)->getIntValue();
					break;
				case KW_CLOSE_ENEMY:
					return true;
					break;
				case KW_CLOSE_SHIP:
					return true;
					break;
				default:
					return false;
			}
		}
	} catch (out_of_range) { }
	return false;
}

static bool createRawCampaignPickup(const vector<Token*> *tokens, int &counter, FileLoader::RawCampaignPickup *rawCampaignPickupOut) {
	try {
		for (; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_X:
					rawCampaignPickupOut->x = tokens->at(++counter)->getIntValue();
					break;
				case KW_Y:
					rawCampaignPickupOut->y = tokens->at(++counter)->getIntValue();
					break;
				case KW_INTERVAL:
					rawCampaignPickupOut->interval = tokens->at(++counter)->getIntValue();
					break;
				case KW_COUNT:
					rawCampaignPickupOut->usageCount = tokens->at(++counter)->getIntValue();
					break;
				case KW_RESPAWN_COUNT:
					rawCampaignPickupOut->respawnCount = tokens->at(++counter)->getIntValue();
					break;
				case KW_PICKUP_START_MOVES:
					if (!createPickupMoves(tokens, ++counter, &rawCampaignPickupOut->moves)) {
						return false;
					}
					break;
				case KW_CLOSE_PICKUP:
					return true;
				default:
					return false;
			}
		}
	} catch (out_of_range) { }
	return false;
}

static bool createPickupMoves(const vector<Token*> *tokens, int &counter, vector<PickupMove> *pickupMovesOut) {
	try {
		for (; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_X:
					pickupMovesOut->push_back((PickupMove)tokens->at(++counter)->getIntValue());
					break;
				case KW_PICKUP_CLOSE_MOVES:
					return true;
				default:
					return false;
			}
		}
	} catch (out_of_range) { }
	return false;
}

static bool createPlayerPosition(const vector<Token*> *tokens, int &counter, Vector2f *playerPositionOut) {
	try {
		for (; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_X:
					playerPositionOut->x = tokens->at(++counter)->getIntValue();
					break;
				case KW_Y:
					playerPositionOut->y = tokens->at(++counter)->getIntValue();
					break;
				case KW_CLOSE_PLAYER_POSITION:
					return true;
				default:
					return false;
			}
		}
	} catch (out_of_range) { }
	return false;
}

static bool createGridOffset(const vector<Token*> *tokens, int &counter, Vector2i *gridOffsetOut) {
	try {
		for (; counter < tokens->size(); ++counter) {
			switch ((*tokens)[counter]->getKeyword()) {
				case KW_X:
					gridOffsetOut->x = tokens->at(++counter)->getIntValue();
					break;
				case KW_Y:
					gridOffsetOut->y = tokens->at(++counter)->getIntValue();
					break;
				case KW_CLOSE_GRID_OFFSET:
					return true;
				default:
					return false;
			}
		}
	} catch (out_of_range) { }
}