#ifndef MUSIC_MANAGER_H
#define MUSIC_MANAGER_H

#include <vector>
#include <string>
#include "StartEngine.h"

class MusicManager {
private:
	enum Mode {
		MODE_GAME, MODE_MENU, MODE_TRANSITION
	}currentMode, modeAfterTransition;

	StartEngine *engine;
	std::vector<std::string> allMusicList, playList;
	std::string menuMusicPath;
	float transitionGain, transitionGainLoss, volume;
	std::string lastPlayed;

	void playNewGameMusic();
	void transitionMode();

public:
	MusicManager(StartEngine *engine);
	
	void update();
	void changeMode(bool inGame);

	void setVolume(float volume);
};

#endif