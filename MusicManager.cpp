#include "MusicManager.h"
#include <fstream>
#include "stringExtension.h"
#include "MT_RandomGenerator.h"
using namespace StringExtension;
using namespace std;

MusicManager::MusicManager(StartEngine *engine) {
	this->engine = engine;
	volume = 1.0f;
	ifstream listFile("Sound/Music/list.txt");
	if (listFile.is_open()) {
		string path;
		while (getLineFromFile(&listFile, &path)) {
			allMusicList.push_back("Sound/Music/" + path);
		}
	}
	menuMusicPath = "Sound/Music/Legacy.ogg";
	transitionGain = volume;
	transitionGainLoss = volume / 90.0f;
	currentMode = MODE_MENU;
}
	
void MusicManager::update() {
	if (!engine->getIsAudioStreaming()) {
		switch (currentMode) {
			case MODE_GAME:
				playNewGameMusic();
				break;
			case MODE_MENU:
				engine->setStreamGain(volume);
				engine->startAudioStreaming(menuMusicPath);
				break;
			default:
				currentMode = modeAfterTransition;
		}
	} else if (currentMode == MODE_TRANSITION) {
		transitionMode();
	}
}

void MusicManager::playNewGameMusic() {
	if (playList.size() == 0) {
		playList = allMusicList;
	} else {
		int musicIndex = MT_RandomGenerator::randomNumber() % playList.size();
		string musicPath = playList[musicIndex];
		if (musicPath != lastPlayed) {
			lastPlayed = musicPath;
			playList.erase(playList.begin() + musicIndex);
			engine->startAudioStreaming(musicPath);
			engine->setStreamGain(volume);
		}
	}
}

void MusicManager::transitionMode() {
	transitionGain -= transitionGainLoss;
	if (transitionGain <= 0.0f) {
		engine->stopAudioStreaming();
		currentMode = modeAfterTransition;
	} else {
		engine->setStreamGain(transitionGain);
	}
}

void MusicManager::changeMode(bool inGame) {
	currentMode = MODE_TRANSITION;
	transitionGain = volume;
	modeAfterTransition = (inGame) ? MODE_GAME : MODE_MENU;
}

void MusicManager::setVolume(float volume) {
	this->volume = volume;
	engine->setStreamGain(volume);
}