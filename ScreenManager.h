#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "Screen.h"
#include "SplashScreen.h"
#include "MainScreen.h"
#include "CharacterSelection.h"
#include "ArenaSelection.h"
#include "SurvivalMode.h"
#include "StartEngine.h"
#include <vector>
#include <functional>
#include "MatchGameManager.h"
#include "CampaignGameManager.h"
#include "DifficultySelection.h"
#include "CampaignDifficultySelection.h"
#include "ChapterSelection.h"
#include "SingleCharacterSelection.h"
#include "MultiplayerMode.h"
#include "HostGame.h"
#include "JoinGame.h"
#include "HostNetworkGameManager.h"
#include "ClientNetworkGameManager.h"
#include "MultiplayerVictory.h"
#include "ResultScreen.h"
#include "CreditScreen.h"
#include "MusicManager.h"
#include "Options.h"

class ScreenManager {
private:
	StartEngine *engine;
	MainScreen mainScreen;
	CharacterSelection characterSelectionScreen;
	ArenaSelection arenaSelection;
	DifficultySelection difficultySelection;
	SingleCharacterSelection *singleCharacterSelection;
	MultiplayerMode multiplayerMode;
	HostGame hostGame;
	JoinGame joinGame;
	MultiplayerVictory multiplayerVictory;
	SurvivalMode survivalMode;
	CampaignDifficultySelection campaignDifficultySelection;
	ChapterSelection chapterSelection;
	ResultScreen resultScreen;
	SplashScreen *splashScreen;
	CreditScreen creditScreen;
	OptionScreen *optionScreen;

	std::vector<Screen*> screens[5];
	int screenIndex, subScreenIndex;
	std::function<void()> render, quitCallBack;
	MatchGameManager matchGameManager;
	CampaignGameManager campaignGameManager;
	HostNetworkGameManager hostNetworkGameManager;
	ClientNetworkGameManager clientNetworkGameManager;
	Image font, shootFrame;
	Image pickupSprites[10], pickupNoRespawn, pickupCross;
	MusicManager musicManager;

	void renderGUIScreen();
	void renderGame();
	void renderCampaignGame();
	void renderHostedGame();
	void renderJoinedGame();

	void renderSinglePlayer();
	void renderMultiplayerMode();
	void renderMultiPlayerOffline();
	void renderMultiPlayerHost();
	void renderMultiPlayerJoin();
	void renderMultiplayerVictory();
	void renderMainScreen();
	void renderSurvivalMode();
	void renderResultScreen();
	void renderSplashScreen();
	void renderCreditScreen();
	void renderOptionsScreen();

	void createSinglePlayerGame(bool survivalMode);
	void createMultiPlayerOfflineGame();
	void createMultiplayerHostedGame();
	void createMultiplayerJoinedGame();
	void backToMain();
	void multiplayerVictoryCallback();
	void unlockChapter();
	void unlockChapter(int chapter);
	void gotoMainScreen();

public:
	ScreenManager(StartEngine *engine, std::function<void()> quitCallBack);
	~ScreenManager();

	void renderScreen();
};

#endif