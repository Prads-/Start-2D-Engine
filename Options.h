#ifndef OPTIONS_H
#define OPTIONS_H

#include "Screen.h"
#include "Button.h"
#include "Spinner.h"
#include "MusicManager.h"
#include <functional>

class OptionScreen : public Screen {
private:
	Button btnBackToMain;
	Spinner spnSfxVolume, spnMusicVolume;
	MusicManager *musicManager;
	std::function<void()> sfxValueChangedCallback, musicValueChangedCallback;
	const Image *font;

	void sfxValueChanged();
	void musicValueChanged();

public:
	OptionScreen(StartEngine *engine, const Image *font, MusicManager *musicManager);

	void update();
	void renderScreen();
};

#endif