#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include "declarations.h"
#include <string>

class ConfigFile {
private:
#pragma pack (push)
#pragma pack (1)
	struct ConfigFileStructure {
		uint8 playerName[20];
		uint16 portNumber;
		uint32 survivalHighScore;
		uint32 screenResolutionX, screenResolutionY;
		uint8 easySavedChapter, easySavedLevel;
		uint8 hardSavedChapter, hardSavedLevel;
		uint32 flags;
		uint8 soundVolumes;
	};
#pragma pack (pop)

	static ConfigFileStructure configFile;

	static void saveConfigFile();

public:
	static void loadConfigFile();

	static std::string getPlayerName();
	static void setPlayerName(std::string name, bool saveCFile = true);
	
	static uint16 getPortNumber();
	static void setPortNumber(uint16 portNumber, bool saveCFile = true);

	static uint32 getSurvivalHighScore();
	static void setSurvivalHighScore(uint32 highscore, bool saveCFile = true);

	static void getScreenResolution(uint32 &xOut, uint32 &yOut);
	static void setScreenResolution(uint32 x, uint32 y, bool saveCFile = true);

	static void getEasySavedChapter(uint8 &chapter, uint8 &level);
	static void setEasySavedChapter(uint8 chapter, uint8 level, bool saveCFile = true);

	static void getHardSavedChapter(uint8 &chapter, uint8 &level);
	static void setHardSavedChapter(uint8 chapter, uint8 level, bool saveCFile = true);

	static bool getIsHardModeUnlocked();
	static void setIsHardModeUnlocked(bool unlocked, bool saveCFile = true);

	static bool getIsChapterUnlocked(int chapter);
	static void setUnlockedChapter(uint8 chapter, bool saveCFile = true);

	static bool getVSync();
	static void setVSync(bool vsync, bool saveCFile = true);

	static bool getMaintainAspectRatio();
	static void setMaintainAspectRatio(bool maintainAspectRatio, bool saveCFile = true);

	static int getSfxVolume();
	static void setSfxVolume(int volume, bool saveCFile = true);
	static int getMusicVolume();
	static void setMusicVolume(int volume, bool saveCFile = true);
};

#endif