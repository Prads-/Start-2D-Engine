#include "ScreenManager.h"
#include "Immobiliser.h"
#include "Explosion.h"
#include <string>
#include "MT_RandomGenerator.h"
#include "stringExtension.h"
#include "ConfigFile.h"
using namespace std;
using namespace StringExtension;

#define _MULTI_PLAYER_OFFLINE								0
#define _SINGLE_PLAYER										1
#define _SURVIVAL_MODE										2
#define _MULTI_PLAYER_HOST									3
#define _MULTI_PLAYER_JOIN									4
#define _MAIN_SCREEN										5
#define _MULTI_PLAYER_MODE									6
#define _MULTI_PLAYER_VICTORY								7
#define _RESULT_SCREEN										8
#define _SPLASH_SCREEN										9
#define _CREDIT_SCREEN										10
#define _OPTIONS_SCREEN										11

#define _SUB_SINGLE_PLAYER_DIFFICULTY_SELECTION				0
#define _SUB_SINGLE_PLAYER_CHARACTER_SELECTION				1
#define _SUB_SINGLE_PLAYER_CHAPTER_SELECTION				2

#define _SUB_MULTI_PLAYER_OFFLINE_CHARACTER_SELECTION		0
#define _SUB_MULTI_PLAYER_OFFLINE_ARENA_SELECTION			1
#define _SUB_MULTI_PLAYER_OFFLINE_DIFFICULTY_SELECTION		2

#define _SUB_MULTI_PLAYER_HOST_CHARACTER_SELECTION			0
#define _SUB_MULTI_PLAYER_HOST_ARENA_SELECTION				1
#define _SUB_MULTI_PLAYER_HOST_CONFIG						2

#define _SUB_MULTI_PLAYER_JOIN_CHARACTER_SELECTION			0
#define _SUB_MULTI_PLAYER_JOIN_CONFIG						1

#define _SUB_SURVIVAL_MODE_CHARACTER_SELECT					0
#define _SUB_SURVIVAL_MODE_START							1


ScreenManager::ScreenManager(StartEngine *engine, std::function<void()> quitCallBack) : mainScreen(engine), 
	characterSelectionScreen(engine, pickupSprites), 
	arenaSelection(engine), 
	matchGameManager(engine, [this]() { backToMain(); }, &font, pickupSprites, &pickupNoRespawn, &pickupCross, &shootFrame),
	difficultySelection(engine),
	survivalMode(engine, &font),
	campaignGameManager(engine, [this]() { backToMain(); }, &font, arenaSelection.getArenas(), &matchGameManager, 
		pickupSprites, &pickupNoRespawn, &pickupCross),
	campaignDifficultySelection(engine), chapterSelection(engine),
	multiplayerMode(engine), hostGame(engine, &font), joinGame(engine, &font),
	hostNetworkGameManager(engine, &font, pickupSprites, &pickupCross, &shootFrame, [this]() { backToMain(); }, [this]() { multiplayerVictoryCallback(); }),
	clientNetworkGameManager(engine, &font, pickupSprites, &pickupCross, &shootFrame, [this]() { backToMain(); }, [this]() { multiplayerVictoryCallback(); }),
	multiplayerVictory(engine, &font), resultScreen(engine, &font), creditScreen(engine, &font),
	musicManager(engine) {
        //, engineSplashScreen(engine), teamSplashScreen(engine) {
	this->engine = engine;
	singleCharacterSelection = new SingleCharacterSelection(engine, characterSelectionScreen.getCharactersList(), pickupSprites);
	pickupCross.loadImage("Images/specialAttacks/pickupItemCross.png");
	shootFrame.loadImage("Images/shootFrame.png");

	screens[_SINGLE_PLAYER].push_back(&campaignDifficultySelection);
	screens[_SINGLE_PLAYER].push_back(singleCharacterSelection);
	screens[_SINGLE_PLAYER].push_back(&chapterSelection);

	screens[_MULTI_PLAYER_OFFLINE].push_back(&characterSelectionScreen);
	screens[_MULTI_PLAYER_OFFLINE].push_back(&arenaSelection);
	screens[_MULTI_PLAYER_OFFLINE].push_back(&difficultySelection);
	
	screens[_MULTI_PLAYER_HOST].push_back(singleCharacterSelection);
	screens[_MULTI_PLAYER_HOST].push_back(&arenaSelection);
	screens[_MULTI_PLAYER_HOST].push_back(&hostGame);
	
	screens[_MULTI_PLAYER_JOIN].push_back(singleCharacterSelection);
	screens[_MULTI_PLAYER_JOIN].push_back(&joinGame);

	screens[_SURVIVAL_MODE].push_back(singleCharacterSelection);
	screens[_SURVIVAL_MODE].push_back(&survivalMode);

	render = [this]() { renderGUIScreen(); };

	font.loadImage("Images/font.png");

	pickupSprites[PICKUP_GRANADE].loadImage("Images/specialAttacks/explosive.png");
	pickupSprites[PICKUP_HOMING].loadImage("Images/specialAttacks/homing.png");
	pickupSprites[PICKUP_IMMOBILISER].loadImage("Images/specialAttacks/stun.png");
	pickupSprites[PICKUP_PINBALL].loadImage("Images/specialAttacks/pinball.png");
	pickupSprites[PICKUP_SHIELD].loadImage("Images/specialAttacks/shield.png");
	pickupSprites[PICKUP_PIERCING_BALL].loadImage("Images/specialAttacks/piercingBall.png");
	pickupSprites[PICKUP_BATTERY].loadImage("Images/specialAttacks/battery.png");
	pickupSprites[PICKUP_INVISIBLE_BALL].loadImage("Images/specialAttacks/invisibleBall.png");
	pickupSprites[PICKUP_TELEPORT_BALL].loadImage("Images/specialAttacks/teleport.png");
	pickupSprites[PICKUP_CALL_BALL].loadImage("Images/specialAttacks/call.png");
	pickupNoRespawn.loadImage("Images/specialAttacks/noRespawn.png");

	screenIndex = _SPLASH_SCREEN;
	subScreenIndex = 0;
	splashScreen = new SplashScreen(engine, 100);

	optionScreen = new OptionScreen(engine, &font, &musicManager);

	this->quitCallBack = quitCallBack;
}

ScreenManager::~ScreenManager() {
	matchGameManager.cleanUpGame();
	Explosion::getInstance()->cleanupInstance();
	delete singleCharacterSelection;
	if (splashScreen) {
		delete splashScreen;
	}
	delete optionScreen;
}

void ScreenManager::renderScreen() {
	render();
}

void ScreenManager::renderGUIScreen() {
	switch (screenIndex) {
		case _MULTI_PLAYER_MODE:
			renderMultiplayerMode();
			break;
		case _MULTI_PLAYER_OFFLINE:
			renderMultiPlayerOffline();
			break;
		case _MULTI_PLAYER_HOST:
			renderMultiPlayerHost();
			break;
		case _MULTI_PLAYER_JOIN:
			renderMultiPlayerJoin();
			break;
		case _MULTI_PLAYER_VICTORY:
			renderMultiplayerVictory();
			break;
		case _SINGLE_PLAYER:
			renderSinglePlayer();
			break;
		case _SURVIVAL_MODE:
			renderSurvivalMode();
			break;
		case _MAIN_SCREEN:
			renderMainScreen();
			break;
		case _RESULT_SCREEN:
			renderResultScreen();
			break;
		case _SPLASH_SCREEN:
			renderSplashScreen();
			return;
		case _CREDIT_SCREEN:
			renderCreditScreen();
			break;
		case _OPTIONS_SCREEN:
			renderOptionsScreen();
	}
	musicManager.update();
}

void ScreenManager::renderGame() {
	GameManager::GameResult gameResult = matchGameManager.getResult();
	if (gameResult == GameManager::GAME_UNDECIDED) {
		matchGameManager.render();
	} else {
		resultScreen.resetScreen();
		if (gameResult == GameManager::GAME_AI_WON) {
			resultScreen.setMessage("Sorry, you've lost the match");
		} else {
			resultScreen.setMessage("Congratulations! You've won the match!!!");
		}
		matchGameManager.cleanUpGame();
		screenIndex = _RESULT_SCREEN;
		subScreenIndex = 0;
		render = [this]() { renderGUIScreen(); };
	}
	musicManager.update();
}

void ScreenManager::renderCampaignGame() {
	GameManager::GameResult gameResult = campaignGameManager.getResult();
	
	switch (gameResult) {
		case GameManager::GAME_UNDECIDED:
			campaignGameManager.render();
			break;
		case GameManager::GAME_PLAYER_WON:
			unlockChapter();
			resultScreen.resetScreen();
			resultScreen.setMessage("Congratulations! You've completed this chapter!!!");
			campaignGameManager.cleanUpGame();
			screenIndex = _RESULT_SCREEN;
			subScreenIndex = 0;
			render = [this]() { renderGUIScreen(); };
			break;
		case GameManager::GAME_SURVIVAL_MODE_ENDED:
			campaignGameManager.cleanUpGame();
			screenIndex = _SURVIVAL_MODE;
			subScreenIndex = _SUB_SURVIVAL_MODE_START;
			survivalMode.newScore(campaignGameManager.getSurvivalModeScore());
			survivalMode.resetScreen();
			render = [this]() { renderGUIScreen(); };
			musicManager.changeMode(false);
	}
	musicManager.update();
}

void ScreenManager::renderHostedGame() {
	hostNetworkGameManager.render();
	musicManager.update();
}

void ScreenManager::renderJoinedGame() {
	clientNetworkGameManager.render();
	musicManager.update();
}

void ScreenManager::renderResultScreen() {
	resultScreen.update();
	resultScreen.renderScreen();
	if (resultScreen.getMoveOn() == Screen::MOVE_ON_PREVIOUS) {
		backToMain();
	}
}

void ScreenManager::renderSplashScreen() {
	splashScreen->update();
	splashScreen->renderScreen();
	if (*(bool*)splashScreen->getUserInput()) {
		delete splashScreen;
		splashScreen = 0;
		gotoMainScreen();
	}
}

void ScreenManager::renderCreditScreen() {
	creditScreen.update();
	creditScreen.renderScreen();
	if (creditScreen.getMoveOn() == Screen::MOVE_ON_PREVIOUS) {
		gotoMainScreen();
	}
}

void ScreenManager::renderOptionsScreen() {
	optionScreen->update();
	optionScreen->renderScreen();
	if (optionScreen->getMoveOn() == Screen::MOVE_ON_PREVIOUS) {
		gotoMainScreen();
	}
}

void ScreenManager::renderSinglePlayer() {
	Screen *screen = screens[_SINGLE_PLAYER][subScreenIndex];
	screen->update();
	screen->renderScreen();
	Screen::MoveOn moveOn = screen->getMoveOn();
	if (moveOn == Screen::MOVE_ON_NEXT) {
		switch (subScreenIndex) {
			case _SUB_SINGLE_PLAYER_DIFFICULTY_SELECTION:
				subScreenIndex = _SUB_SINGLE_PLAYER_CHARACTER_SELECTION;
				singleCharacterSelection->resetScreen();
				break;
			case _SUB_SINGLE_PLAYER_CHARACTER_SELECTION:
				subScreenIndex = _SUB_SINGLE_PLAYER_CHAPTER_SELECTION;
				chapterSelection.resetScreen();
				break;
			default:
				createSinglePlayerGame(false);
		}
	} else if (moveOn == Screen::MOVE_ON_PREVIOUS) {
		switch (subScreenIndex) {
			case _SUB_SINGLE_PLAYER_DIFFICULTY_SELECTION:
				gotoMainScreen();
				break;
			case _SUB_SINGLE_PLAYER_CHARACTER_SELECTION:
				subScreenIndex = _SUB_SINGLE_PLAYER_DIFFICULTY_SELECTION;
				campaignDifficultySelection.resetScreen();
				break;
			default:
				subScreenIndex = _SUB_SINGLE_PLAYER_CHARACTER_SELECTION;
				singleCharacterSelection->resetScreen();
		}
	}
}

void ScreenManager::renderMultiplayerMode() {
	multiplayerMode.update();
	multiplayerMode.renderScreen();
	switch (multiplayerMode.getMoveOn()) {
		case Screen::MOVE_ON_NEXT:
			switch(*(MultiplayerMode::MultiplayerModeOption*)multiplayerMode.getUserInput()) {
				case MultiplayerMode::MULTIPLAYER_MODE_HOST:
					screenIndex = _MULTI_PLAYER_HOST;
					subScreenIndex = _SUB_MULTI_PLAYER_HOST_CHARACTER_SELECTION;
					singleCharacterSelection->resetScreen();
					break;
				case MultiplayerMode::MULTIPLAYER_MODE_JOIN:
					screenIndex = _MULTI_PLAYER_JOIN;
					subScreenIndex = _SUB_MULTI_PLAYER_JOIN_CHARACTER_SELECTION;
					singleCharacterSelection->resetScreen();
					break;
				default:
					screenIndex = _MULTI_PLAYER_OFFLINE;
					subScreenIndex = _SUB_MULTI_PLAYER_OFFLINE_CHARACTER_SELECTION;
					characterSelectionScreen.resetScreen();
			}
			break;
		case Screen::MOVE_ON_PREVIOUS:
			gotoMainScreen();
	}
}

void ScreenManager::renderMultiPlayerOffline() {
	Screen *screen = screens[_MULTI_PLAYER_OFFLINE][subScreenIndex];
	screen->update();
	screen->renderScreen();
	Screen::MoveOn moveOn = screen->getMoveOn();
	if (moveOn == Screen::MOVE_ON_NEXT) {
		if (subScreenIndex == _SUB_MULTI_PLAYER_OFFLINE_CHARACTER_SELECTION) {
			subScreenIndex = _SUB_MULTI_PLAYER_OFFLINE_ARENA_SELECTION;
			arenaSelection.resetScreen();
		} else if (subScreenIndex == _SUB_MULTI_PLAYER_OFFLINE_ARENA_SELECTION) {
			subScreenIndex = _SUB_MULTI_PLAYER_OFFLINE_DIFFICULTY_SELECTION;
			difficultySelection.resetScreen();
		} else {
			createMultiPlayerOfflineGame();
		}
	} else if (moveOn == Screen::MOVE_ON_PREVIOUS) {
		if (subScreenIndex == _SUB_MULTI_PLAYER_OFFLINE_CHARACTER_SELECTION) {
			screenIndex = _MULTI_PLAYER_MODE;
			multiplayerMode.resetScreen();
		} else if (subScreenIndex == _SUB_MULTI_PLAYER_OFFLINE_ARENA_SELECTION) {
			subScreenIndex = _SUB_MULTI_PLAYER_OFFLINE_CHARACTER_SELECTION;
			characterSelectionScreen.resetScreen();
		} else {
			subScreenIndex = _SUB_MULTI_PLAYER_OFFLINE_ARENA_SELECTION;
			arenaSelection.resetScreen();
		}
	}
}

void ScreenManager::renderMultiPlayerHost() {
	Screen *screen = screens[_MULTI_PLAYER_HOST][subScreenIndex];
	screen->update();
	screen->renderScreen();
	switch (screen->getMoveOn()) {
		case Screen::MOVE_ON_NEXT:
			switch (subScreenIndex) {
				case _SUB_MULTI_PLAYER_HOST_CHARACTER_SELECTION:
					subScreenIndex = _SUB_MULTI_PLAYER_HOST_ARENA_SELECTION;
					arenaSelection.resetScreen();
					break;
				case _SUB_MULTI_PLAYER_HOST_ARENA_SELECTION:
					subScreenIndex = _SUB_MULTI_PLAYER_HOST_CONFIG;
					hostGame.resetScreen();
					break;
				case _SUB_MULTI_PLAYER_HOST_CONFIG:
					createMultiplayerHostedGame();
					break;
			}
			break;
		case Screen::MOVE_ON_PREVIOUS:
			switch (subScreenIndex) {
				case _SUB_MULTI_PLAYER_HOST_CHARACTER_SELECTION:
					screenIndex = _MULTI_PLAYER_MODE;
					multiplayerMode.resetScreen();
					break;
				case _SUB_MULTI_PLAYER_HOST_ARENA_SELECTION:
					subScreenIndex = _SUB_MULTI_PLAYER_HOST_CHARACTER_SELECTION;
					singleCharacterSelection->resetScreen();
					break;
				case _SUB_MULTI_PLAYER_HOST_CONFIG:
					subScreenIndex = _SUB_MULTI_PLAYER_HOST_ARENA_SELECTION;
					arenaSelection.resetScreen();
					break;
			}
	}
}

void ScreenManager::renderMultiPlayerJoin() {
	Screen *screen = screens[_MULTI_PLAYER_JOIN][subScreenIndex];
	screen->update();
	screen->renderScreen();
	switch (screen->getMoveOn()) {
		case Screen::MOVE_ON_NEXT:
			switch (subScreenIndex) {
				case _SUB_MULTI_PLAYER_JOIN_CHARACTER_SELECTION:
					subScreenIndex = _SUB_MULTI_PLAYER_JOIN_CONFIG;
					joinGame.resetScreen();
					break;
				case _SUB_MULTI_PLAYER_JOIN_CONFIG:
					createMultiplayerJoinedGame();
					break;
			}
			break;
		case Screen::MOVE_ON_PREVIOUS:
			switch (subScreenIndex) {
				case _SUB_MULTI_PLAYER_JOIN_CHARACTER_SELECTION:
					screenIndex = _MULTI_PLAYER_MODE;
					multiplayerMode.resetScreen();
					break;
				case _SUB_MULTI_PLAYER_JOIN_CONFIG:
					subScreenIndex = _SUB_MULTI_PLAYER_JOIN_CHARACTER_SELECTION;
					singleCharacterSelection->resetScreen();
					break;
			}
	}
}

void ScreenManager::renderMultiplayerVictory() {
	multiplayerVictory.update();
	multiplayerVictory.renderScreen();
	if (multiplayerVictory.getMoveOn() == Screen::MOVE_ON_PREVIOUS) {
		gotoMainScreen();
	}
}

void ScreenManager::renderMainScreen() {
	mainScreen.update();
	mainScreen.renderScreen();
	MainScreen::MainScreenOutput *userInput = (MainScreen::MainScreenOutput*)mainScreen.getUserInput();
	switch (*userInput) {
		case MainScreen::MAIN_SCREEN_MULTIPLAYER:
			screenIndex = _MULTI_PLAYER_MODE;
			multiplayerMode.resetScreen();
			break;
		case MainScreen::MAIN_SCREEN_SINGLE_PLAYER:
			screenIndex = _SINGLE_PLAYER;
			subScreenIndex = _SUB_SINGLE_PLAYER_DIFFICULTY_SELECTION;
			campaignDifficultySelection.resetScreen();
			break;
		case MainScreen::MAIN_SCREEN_SURVIVAL_MODE:
			screenIndex = _SURVIVAL_MODE;
			subScreenIndex = _SUB_SURVIVAL_MODE_CHARACTER_SELECT;
			singleCharacterSelection->resetScreen();
			break;
		case MainScreen::MAIN_SCREEN_QUIT:
			quitCallBack();
			break;
		case MainScreen::MAIN_SCREEN_CREDITS:
			screenIndex = _CREDIT_SCREEN;
			subScreenIndex = 0;
			creditScreen.resetScreen();
			break;
		case MainScreen::MAIN_SCREEN_OPTIONS:
			screenIndex = _OPTIONS_SCREEN;
			subScreenIndex = 0;
			optionScreen->resetScreen();
	}
}

void ScreenManager::renderSurvivalMode() {
	Screen *screen = screens[_SURVIVAL_MODE][subScreenIndex];
	screen->update();
	screen->renderScreen();
	Screen::MoveOn moveOn = screen->getMoveOn();
	if (moveOn == Screen::MOVE_ON_NEXT) {
		if (subScreenIndex == _SUB_SURVIVAL_MODE_CHARACTER_SELECT) {
			subScreenIndex = _SUB_SURVIVAL_MODE_START;
			survivalMode.resetScreen();
		} else {
			createSinglePlayerGame(true);
		}
	} else if (moveOn == Screen::MOVE_ON_PREVIOUS) {
		if (subScreenIndex == _SUB_SURVIVAL_MODE_CHARACTER_SELECT) {
			gotoMainScreen();
		} else {
			subScreenIndex = _SUB_SURVIVAL_MODE_CHARACTER_SELECT;
			singleCharacterSelection->resetScreen();
		}
	}
}

void ScreenManager::createMultiPlayerOfflineGame() {
	GameManager::GameInformation gameInformation;
	const CharacterSelection::SelectedCharacter *selectedCharacters = (const CharacterSelection::SelectedCharacter*)characterSelectionScreen.getUserInput();
	gameInformation.player = selectedCharacters[0].character;
	for (int i = 1; i < 4; ++i) {
		gameInformation.ai[i - 1] = selectedCharacters[i].character;
	}
	gameInformation.difficulty = *(AIDifficulty*)difficultySelection.getUserInput();
	Arena *arena = ((Arena*)arenaSelection.getUserInput());
	if (arena == 0) {
		matchGameManager.startRandomLevel(gameInformation);
	} else {
		gameInformation.path = *arena->getPath();
		matchGameManager.startGame(gameInformation);
	}
	render = [this]() { renderGame(); };
	musicManager.changeMode(true);
}

void ScreenManager::createMultiplayerHostedGame() {
	const CharacterSelection::SelectedCharacter *selectedCharacter = (CharacterSelection::SelectedCharacter*)singleCharacterSelection->getUserInput();
	const Arena *arena = (Arena*)arenaSelection.getUserInput();
	const HostGame::HostGameInformation *hostGameInformation = (HostGame::HostGameInformation*)hostGame.getUserInput();

	HostNetworkGameManager::NetworkGameInformation gameInformation;
	gameInformation.aiDifficulty = AI_FAST;
	gameInformation.allocateAIToEmptyPlace = hostGameInformation->bots;
	gameInformation.hostCharacterId = selectedCharacter->character->getId();
	if (arena) {
		gameInformation.mapId = arena->getId();
		gameInformation.mapPath = *arena->getPath();
	} else {
		gameInformation.mapId = -1;
	}
	gameInformation.numberOfPlayers = hostGameInformation->numberOfPlayers;
	gameInformation.nickname = hostGameInformation->nickName;
	gameInformation.portNumber = hostGameInformation->port;

	hostNetworkGameManager.startNetworkGame(gameInformation);
	render = [this]() { renderHostedGame(); };
	musicManager.changeMode(true);
}

void ScreenManager::createMultiplayerJoinedGame() {
	const CharacterSelection::SelectedCharacter *selectedCharacter = (CharacterSelection::SelectedCharacter*)singleCharacterSelection->getUserInput();
	const JoinGame::JoinGameInformation *joinGameInformation = (JoinGame::JoinGameInformation*)joinGame.getUserInput();

	ClientNetworkGameManager::NetworkGameInformation gameInformation;
	gameInformation.characterId = selectedCharacter->character->getId();
	gameInformation.nickname = joinGameInformation->nickname;
	gameInformation.portNumber = joinGameInformation->port;

	clientNetworkGameManager.startNetworkGame(gameInformation);
	render = [this]() { renderJoinedGame(); };
	musicManager.changeMode(true);
}

void ScreenManager::createSinglePlayerGame(bool survivalMode) {
	GameManager::GameInformation gameInformation;
	
	gameInformation.player = ((CharacterSelection::SelectedCharacter*)singleCharacterSelection->getUserInput())->character;
	
	if (survivalMode) {
		campaignGameManager.startRandomLevel(gameInformation);
	} else {
		string levelPath;
		CampaignDifficultySelection::CampaignDifficulty campaignDifficulty = *(CampaignDifficultySelection::CampaignDifficulty*)campaignDifficultySelection.getUserInput();
		if (campaignDifficulty == CampaignDifficultySelection::EASY) {
			levelPath = "Easy/";
		} else {
			levelPath = "Hard/";
		}
		uint8 chapterId = *(int*)chapterSelection.getUserInput(), levelOffet;
		if (chapterId == CHAPTER_SELECT_LOAD_SAVED) {
			if (campaignDifficulty == CampaignDifficultySelection::EASY) {
				ConfigFile::getEasySavedChapter(chapterId, levelOffet);
			} else {
				ConfigFile::getHardSavedChapter(chapterId, levelOffet);
			}
		} else {
			levelOffet = 0;
		}
		levelPath += intToStr(chapterId) + ".txt";
		gameInformation.path = levelPath;
		campaignGameManager.startGame(gameInformation, levelOffet, chapterId, campaignDifficulty == CampaignDifficultySelection::HARD);
	}
	render = [this]() { renderCampaignGame(); };
	musicManager.changeMode(true);
}

void ScreenManager::backToMain() {
	gotoMainScreen();
	render = [this]() { renderGUIScreen(); };
	musicManager.changeMode(false);
}

void ScreenManager::multiplayerVictoryCallback() {
	screenIndex = _MULTI_PLAYER_VICTORY;
	subScreenIndex = 0;
	multiplayerVictory.resetScreen();
	render = [this]() { renderGUIScreen(); };
	musicManager.changeMode(false);
}

void ScreenManager::unlockChapter() {
	if (!ConfigFile::getIsHardModeUnlocked()) {
		int chapter = *(int*)chapterSelection.getUserInput();
		if (chapter == CHAPTER_SELECT_LOAD_SAVED) {
			CampaignDifficultySelection::CampaignDifficulty difficulty = *(CampaignDifficultySelection::CampaignDifficulty*)campaignDifficultySelection.getUserInput();
			uint8 chapter, level;
			if (difficulty == CampaignDifficultySelection::EASY) {
				ConfigFile::getEasySavedChapter(chapter, level);
			} else {
				ConfigFile::getHardSavedChapter(chapter, level);
			}
			unlockChapter(chapter);
		} else {
			unlockChapter(chapter);
		}
	}
}

void ScreenManager::unlockChapter(int chapter) {
	switch (chapter) {
		case 1:
			if (!ConfigFile::getIsChapterUnlocked(1)) {
				ConfigFile::setUnlockedChapter(1);
			}
			break;
		case 2:
			if (!ConfigFile::getIsChapterUnlocked(2)) {
				ConfigFile::setUnlockedChapter(2);
			}
			break;
		case 3:
			if (!ConfigFile::getIsChapterUnlocked(3)) {
				ConfigFile::setUnlockedChapter(3);
			}
			break;
		case 4:
			ConfigFile::setIsHardModeUnlocked(true);
	}
}

void ScreenManager::gotoMainScreen() {
	screenIndex = _MAIN_SCREEN;
	subScreenIndex = 0;
	mainScreen.resetScreen();
}