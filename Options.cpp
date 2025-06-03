#include "Options.h"
#include "SfxPool.h"
#include "ConfigFile.h"

OptionScreen::OptionScreen(StartEngine *engine, const Image *font, MusicManager *musicManager) : Screen(engine),
		btnBackToMain(engine, Vector2i(0, 709), true, [this]() { previousScreen(); }),
		spnMusicVolume(engine, Vector2i(512, 150), 70, false, 0, 10, 0xFF00, 0xFFFF7F, font),
		spnSfxVolume(engine, Vector2i(512, 190), 70, false, 0, 10, 0xFF00, 0xFFFF7F, font) {
	this->musicManager = musicManager;
	
	btnBackToMain.setButtonSprite(&edgeButtonBackgroundDefault);
	btnBackToMain.setMouseOverSprite(&edgeButtonBackgroundHighlight);
	btnBackToMain.setSfxMouseOver(SfxPool::getSfxButtonOver1());
	btnBackToMain.setCaption("back to main", 50, 5, 0, &customFont, 0xF2D8D8, 0xEDBDA3);

	sfxValueChangedCallback = [this]() { sfxValueChanged(); };
	musicValueChangedCallback = [this]() { musicValueChanged(); };

	spnMusicVolume.setValueChangedCallback(&musicValueChangedCallback);
	spnSfxVolume.setValueChangedCallback(&sfxValueChangedCallback);

	spnMusicVolume.setValue(ConfigFile::getMusicVolume());
	spnSfxVolume.setValue(ConfigFile::getSfxVolume());
	musicManager->setVolume((float)ConfigFile::getMusicVolume() / 10.0f);
	engine->setSourcesGain((float)ConfigFile::getSfxVolume() / 10.0f);

	this->font = font;
}

void OptionScreen::sfxValueChanged() {
	engine->setSourcesGain((float)spnSfxVolume.getValue() / 10.0f);
	engine->playAudioBuffer(SfxPool::getSfxButtonOver1());
	ConfigFile::setSfxVolume(spnSfxVolume.getValue());
}

void OptionScreen::musicValueChanged() {
	musicManager->setVolume((float)spnMusicVolume.getValue() / 10.0f);
	ConfigFile::setMusicVolume(spnMusicVolume.getValue());
}

void OptionScreen::update() {
	bool wasMouseLeftToggled = engine->wasMouseLeftToggled();
	btnBackToMain.update(wasMouseLeftToggled);
	spnMusicVolume.update(wasMouseLeftToggled);
	spnSfxVolume.update(wasMouseLeftToggled);
}

void OptionScreen::renderScreen() {
	engine->draw(&background, 0, false, false, false, 0, 0);
	
	engine->drawText("options", getCustomFont(), 16, 5, 0xFFFF00, 439, 10);
	engine->drawText("Music Volume", font, 32, 32, 16, 0xFFFF00, 40, 155);
	engine->drawText("SFX Volume", font, 32, 32, 16, 0xFFFF00, 40, 195);
	btnBackToMain.draw();
	spnMusicVolume.draw();
	spnSfxVolume.draw();

	engine->presentScreen();
}