#include "FileParser.h"
#include <stdexcept>
#include "TokenKeyword.h"
#include "TokenBool.h"
#include "TokenInteger.h"
#include "TokenString.h"
#include "TokenFloat.h"
#include "stringExtension.h"
using namespace std;
using namespace StringExtension;

static Keyword getStartEndKeyWord(string line);
static Keyword getBodyKeyword(string line, int &counter);
static string getValue(string line, int counter);
static void pushBackStartEndKeyword(string line, vector<Token*> *tokens);

std::vector<Token*> FileParser::parseEnvironmentFileString(std::vector<std::string> *srcLines) {
	vector<Token*> retVal;

	for (string &line : *srcLines) {
		try {
			if (line.at(0) == '[') {
				pushBackStartEndKeyword(line, &retVal);
			} else {
				int counter = 0;
				Keyword kw = getBodyKeyword(line, counter);
				if (kw != KW_NONE) {
					TokenKeyword *keyword = new TokenKeyword();
					keyword->setKeyword(kw);
					retVal.push_back(keyword);

					string strVal = getValue(line, counter);
					switch (kw) {
						case KW_MAX_X:
						case KW_MAX_Y:
						case KW_MIN_X:
						case KW_MIN_Y:
						case KW_ROTATION_ANGLE:
						case KW_X:
						case KW_Y:
							{
								int val = strToInt(strVal);
								TokenInteger *tokenInteger = new TokenInteger();
								tokenInteger->setIntValue(val);
								retVal.push_back(tokenInteger);
							}
							break;
						case KW_FLIP_X:
						case KW_FLIP_Y:
						case KW_ALPHA_BLEND:
						case KW_IS_PRIORITISED:
							{
								bool val = strToInt(strVal);
								TokenBool *tokenBool = new TokenBool();
								tokenBool->setBoolValue(val);
								retVal.push_back(tokenBool);
							}
							break;
						case KW_PATH:
							{
								TokenString *tokenString = new TokenString();
								tokenString->setStringValue(strVal);
								retVal.push_back(tokenString);
							}
							break;
					}
				}
			}
		} catch (out_of_range) { }
	}

	return retVal;
}

std::vector<Token*> FileParser::parseCharacterFileString(std::vector<std::string> *srcLines) {
	vector<Token*> retVal;
	
	for (string &line : *srcLines) {
		int counter = 0;
		Keyword kw = getBodyKeyword(line, counter);
		if (kw != KW_NONE) {
			TokenKeyword *keyword = new TokenKeyword();
			keyword->setKeyword(kw);
			retVal.push_back(keyword);
			string strVal = getValue(line, counter);
			switch (kw) {
				case KW_NAME:
					{
						TokenString *tokenString = new TokenString();
						tokenString->setStringValue(strVal);
						retVal.push_back(tokenString);
					}
					break;
				case KW_WIDTH:
				case KW_HEIGHT:
				case KW_SPECIAL_THROW:
				case KW_SECONDARY_ABILITY:
					{
						TokenInteger *tokenInteger = new TokenInteger();
						tokenInteger->setIntValue(strToInt(strVal));
						retVal.push_back(tokenInteger);
					}
					break;
				case KW_MAX_SPEED:
				case KW_CAUTION_DISTANCE:
				case KW_SHOOTING_SHARPNESS:
					{
						TokenFloat *tokenFloat = new TokenFloat();
						tokenFloat->setFloatValue(atof(strVal.c_str()));
						retVal.push_back(tokenFloat);
					}
			}
		}
	}

	return retVal;
}

std::vector<Token*> FileParser::parseLevelFileString(std::vector<std::string> *srcLines) {
	vector<Token*> retVal;

	for (string &line : *srcLines) {
		try {
			if (line.at(0) == '[') {
				pushBackStartEndKeyword(line, &retVal);
			} else {
				int counter = 0;
				Keyword kw = getBodyKeyword(line, counter);
				if (kw != KW_NONE) {
					TokenKeyword *keyword = new TokenKeyword();
					keyword->setKeyword(kw);
					retVal.push_back(keyword);
					string strVal = getValue(line, counter);
					switch (kw) {
						case KW_ID:
						case KW_X:
						case KW_Y:
						case KW_INTERVAL:
						case KW_COUNT:
						case KW_RESPAWN_COUNT:
							{
								TokenInteger *tokenInteger = new TokenInteger();
								tokenInteger->setIntValue(strToInt(strVal));
								retVal.push_back(tokenInteger);
							}
							break;
					}
				}
			}
		} catch (out_of_range) { }
	}

	return retVal;
}

static Keyword getStartEndKeyWord(string line) {
	if (line == "[Asset]") {
		return KW_START_ASSET;
	} else if (line == "[/Asset]") {
		return KW_CLOSE_ASSET;
	} else if (line == "[BB]") {
		return KW_START_BB;
	} else if (line == "[/BB]") {
		return KW_CLOSE_BB;
	} else if (line == "[Clone]") {
		return KW_START_CLONE;
	} else if (line == "[/Clone]") {
		return KW_CLOSE_CLONE;
	} else if (line == "[Wave]") {
		return KW_START_WAVE;
	} else if (line == "[/Wave]") {
		return KW_CLOSE_WAVE;
	} else if (line == "[Ship]") {
		return KW_START_SHIP;
	} else if (line == "[/Ship]") {
		return KW_CLOSE_SHIP;
	} else if (line == "[Enemy]") {
		return KW_START_ENEMY;
	} else if (line == "[/Enemy]") {
		return KW_CLOSE_ENEMY;
	} else if (line == "[Pickup]") {
		return KW_START_PICKUP;
	} else if (line == "[/Pickup]") {
		return KW_CLOSE_PICKUP;
	} else if (line == "[PickupMoves]") {
		return KW_PICKUP_START_MOVES;
	} else if (line == "[/PickupMoves]") {
		return KW_PICKUP_CLOSE_MOVES;
	} else if (line == "[PlayerPosition]") {
		return KW_START_PLAYER_POSITION;
	} else if (line == "[/PlayerPosition]") {
		return KW_CLOSE_PLAYER_POSITION;
	} else if (line == "[GridOffset]") {
		return KW_START_GRID_OFFSET;
	} else if (line == "[/GridOffset]") {
		return KW_CLOSE_GRID_OFFSET;
	}

	return KW_NONE;
}

static Keyword getBodyKeyword(string line, int &counter) {
	string kw;
	
	while (isalpha(line[counter])) {
		kw += line[counter++];
	}

	if (kw == "Path") {
		return KW_PATH;
	} else if (kw == "RotationAngle") {
		return KW_ROTATION_ANGLE;
	} else if (kw == "FlipX") {
		return KW_FLIP_X;
	} else if (kw == "FlipY") {
		return KW_FLIP_Y;
	} else if (kw == "AlphaBlend") {
		return KW_ALPHA_BLEND;
	} else if (kw == "X") {
		return KW_X;
	} else if (kw == "Y") {
		return KW_Y;
	} else if (kw == "IsPrioritised") {
		return KW_IS_PRIORITISED;
	} else if (kw == "MinX") {
		return KW_MIN_X;
	} else if (kw == "MaxX") {
		return KW_MAX_X;
	} else if (kw == "MinY") {
		return KW_MIN_Y;
	} else if (kw == "MaxY") {
		return KW_MAX_Y;
	} else if (kw == "Name") {
		return KW_NAME;
	} else if (kw == "Width") {
		return KW_WIDTH;
	} else if (kw == "Height") {
		return KW_HEIGHT;
	} else if (kw == "MaxSpeed") {
		return KW_MAX_SPEED;
	} else if (kw == "CautionDistance") {
		return KW_CAUTION_DISTANCE;
	} else if (kw == "ShootingSharpness") {
		return KW_SHOOTING_SHARPNESS;
	} else if (kw == "SpecialThrow") {
		return KW_SPECIAL_THROW;
	} else if (kw == "SecondaryAbility") {
		return KW_SECONDARY_ABILITY;
	} else if (kw == "Id") {
		return KW_ID;
	} else if (kw == "Interval") {
		return KW_INTERVAL;
	} else if (kw == "Count") {
		return KW_COUNT;
	} else if (kw == "RespawnCount") {
		return KW_RESPAWN_COUNT;
	}

	return KW_NONE;
}

static string getValue(string line, int counter) {
	while (!isalnum(line.at(counter))) {
		++counter;
	}

	return line.substr(counter, line.size() - counter);
}

static void pushBackStartEndKeyword(string line, vector<Token*> *tokens) {
	Keyword kw = getStartEndKeyWord(line);
	if (kw != KW_NONE) {
		TokenKeyword *tokenKeyword = new TokenKeyword();
		tokenKeyword->setKeyword(kw);
		tokens->push_back(tokenKeyword);
	}
}