#include "ConfigFile.h"
#include <fstream>
#include "CustomException.h"
using namespace std;

#define MASK_HARD_MODE_UNLOCKED				4
#define MASK_CHAPTER_UNLOCKED				3
#define MASK_VSYNC							8
#define MASK_MAINTAIN_ASPECT_RATIO			0x10
#define MASK_VOL_SFX						0xF
#define MASK_VOL_MUSIC						0xF0

ConfigFile::ConfigFileStructure ConfigFile::configFile;

void ConfigFile::loadConfigFile() {
	ifstream configFile;
	configFile.open("config.dat", ios::binary);
	if (configFile.is_open()) {
		configFile.read((char*)&ConfigFile::configFile, sizeof(ConfigFile::configFile));
		configFile.close();
	} else {
		throw CustomException("Error opening config file");
	}
}

void ConfigFile::saveConfigFile() {
	ofstream configFile;
	configFile.open("config.dat", ios::binary);
	configFile.write((char*)&ConfigFile::configFile, sizeof(ConfigFile::configFile));
	configFile.close();
}

std::string ConfigFile::getPlayerName() {
	string playerName;
	for (int i = 0; i < 20; ++i) {
		if (configFile.playerName[i] != 0) {
			playerName.push_back(configFile.playerName[i]);
		}
	}
	return playerName;
}

void ConfigFile::setPlayerName(std::string name, bool saveCFile) {
	memset(configFile.playerName, 0, 20);
	for (int i = 0; i < 20 && i < name.size(); ++i) {
		configFile.playerName[i] = name[i];
	}
	if (saveCFile) {
		saveConfigFile();
	}
}
	
uint16 ConfigFile::getPortNumber() {
	return configFile.portNumber;
}

void ConfigFile::setPortNumber(uint16 portNumber, bool saveCFile) {
	configFile.portNumber = portNumber;
	if (saveCFile) {
		saveConfigFile();
	}
}

uint32 ConfigFile::getSurvivalHighScore() {
	return configFile.survivalHighScore;
}

void ConfigFile::setSurvivalHighScore(uint32 highscore, bool saveCFile) {
	configFile.survivalHighScore = highscore;
	if (saveCFile) {
		saveConfigFile();
	}
}

void ConfigFile::getScreenResolution(uint32 &xOut, uint32 &yOut) {
	xOut = configFile.screenResolutionX;
	yOut = configFile.screenResolutionY;
}

void ConfigFile::setScreenResolution(uint32 x, uint32 y, bool saveCFile) {
	configFile.screenResolutionX = x;
	configFile.screenResolutionY = y;
	if (saveCFile) {
		saveConfigFile();
	}
}

void ConfigFile::getEasySavedChapter(uint8 &chapter, uint8 &level) {
	chapter = configFile.easySavedChapter;
	level = configFile.easySavedLevel;
}

void ConfigFile::setEasySavedChapter(uint8 chapter, uint8 level, bool saveCFile) {
	configFile.easySavedChapter = chapter;
	configFile.easySavedLevel = level;
	if (saveCFile) {
		saveConfigFile();
	}
}

void ConfigFile::getHardSavedChapter(uint8 &chapter, uint8 &level) {
	chapter = configFile.hardSavedChapter;
	level = configFile.hardSavedLevel;
}

void ConfigFile::setHardSavedChapter(uint8 chapter, uint8 level, bool saveCFile) {
	configFile.hardSavedChapter = chapter;
	configFile.hardSavedLevel = level;
	if (saveCFile) {
		saveConfigFile();
	}
}

bool ConfigFile::getIsHardModeUnlocked() {
	return configFile.flags & MASK_HARD_MODE_UNLOCKED;
}

void ConfigFile::setIsHardModeUnlocked(bool unlocked, bool saveCFile) {
	if (unlocked) {
		configFile.flags |= MASK_HARD_MODE_UNLOCKED;
	} else {
		configFile.flags &= ~MASK_HARD_MODE_UNLOCKED;
	}
	if (saveCFile) {
		saveConfigFile();
	}
}

bool ConfigFile::getIsChapterUnlocked(int chapter) {
	switch (chapter) {
		case 0:
			return true;
		case 1:
			return (configFile.flags & MASK_CHAPTER_UNLOCKED) > 0;
		case 2:
			return (configFile.flags & MASK_CHAPTER_UNLOCKED) > 1;
		case 3:
			return (configFile.flags & MASK_CHAPTER_UNLOCKED) > 2;
	}
	return false;
}

void ConfigFile::setUnlockedChapter(uint8 chapter, bool saveCFile) {
	chapter &= MASK_CHAPTER_UNLOCKED;
	configFile.flags &= ~MASK_CHAPTER_UNLOCKED;
	configFile.flags |= chapter;
	if (saveCFile) {
		saveConfigFile();
	}
}

bool ConfigFile::getVSync() {
	return configFile.flags & MASK_VSYNC;
}

void ConfigFile::setVSync(bool vsync, bool saveCFile) {
	if (vsync) {
		configFile.flags |= MASK_VSYNC;
	} else {
		configFile.flags &= ~MASK_VSYNC;
	}
	if (saveCFile) {
		saveConfigFile();
	}
}

bool ConfigFile::getMaintainAspectRatio() {
	return configFile.flags & MASK_MAINTAIN_ASPECT_RATIO;
}

void ConfigFile::setMaintainAspectRatio(bool maintainAspectRatio, bool saveCFile) {
	if (maintainAspectRatio) {
		configFile.flags |= MASK_MAINTAIN_ASPECT_RATIO;
	} else {
		configFile.flags &= ~MASK_MAINTAIN_ASPECT_RATIO;
	}
	if (saveCFile) {
		saveConfigFile();
	}
}

int ConfigFile::getSfxVolume() {
	return configFile.soundVolumes & MASK_VOL_SFX;
}

void ConfigFile::setSfxVolume(int volume, bool saveCFile) {
	configFile.soundVolumes &= MASK_VOL_MUSIC;
	volume &= MASK_VOL_SFX;
	configFile.soundVolumes |= volume;
	if (saveCFile) {
		saveConfigFile();
	}
}

int ConfigFile::getMusicVolume() {
	return configFile.soundVolumes >> 4;
}

void ConfigFile::setMusicVolume(int volume, bool saveCFile) {
	configFile.soundVolumes &= MASK_VOL_SFX;
	volume <<= 4;
	configFile.soundVolumes |= volume;
	if (saveCFile) {
		saveConfigFile();
	}
}