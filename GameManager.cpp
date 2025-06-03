#include "GameManager.h"
#include "FileLoader.h"
#include "Screen.h"
#include "SfxPool.h"
using namespace FileLoader;
using namespace std;

Image GameManager::resumeButtonImage("Images/Screens/Commons/Resume.png");
Image GameManager::ready("Images/ready.png");

GameManager::GameManager(StartEngine *engine, std::function<void()> backToMainCallBack, Image *font, Image *pickupSprites, Image *pickupNoRespawn) : 
			btnResume(engine, Vector2i(352, 250), true, [this]() { resumeGame(); }), 
			btnBackToMain(engine, Vector2i(352, 332), true, [this]() { goBackToMainScreen(); }) {
    player = 0;
	ball = 0;
	this->engine = engine;
	this->backToMainCallBack = backToMainCallBack;
	this->font = font;

	this->pickupSprites = pickupSprites;
	this->pickupNoRespawn = pickupNoRespawn;

	btnResume.setButtonSprite(Screen::getMiddleButtonBackgroundDefault2());
	btnResume.setMouseOverSprite(Screen::getMiddleButtonBackgroundHighlight());
	btnResume.setCaption("resume", 89, 16, 0, Screen::getCustomFont(), 0xF2D800, 0xEDBD00);
	btnResume.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	btnBackToMain.setButtonSprite(Screen::getMiddleButtonBackgroundDefault2());
	btnBackToMain.setMouseOverSprite(Screen::getMiddleButtonBackgroundHighlight());
	btnBackToMain.setCaption("back to main", 24, 16, 0, Screen::getCustomFont(), 0xF2D800, 0xEDBD00);
	btnBackToMain.setSfxMouseOver(SfxPool::getSfxButtonOver1());

	explosion = Explosion::getInstance(engine);

	gamePaused = false;
    this->backToMainCallBack = backToMainCallBack;
}

GameManager::~GameManager() { }

void GameManager::renderPrioritisedSprites() {
	for (const PrioritisedAsset &asset : prioritisedAsset) {
		renderPrioritisedSprite(&asset);
	}
}

void GameManager::pushBackEdgeBB() {
	bBoxes.push_back(BoundingBox(Vector2f(0.0f, 0.0f), Vector2f(1024.0f, 20.0f)));
	bBoxes.push_back(BoundingBox(Vector2f(0.0f, 748.0f), Vector2f(1024.0f, 768.0f)));
	bBoxes.push_back(BoundingBox(Vector2f(0.0f, 0.0f), Vector2f(20.0f, 768.0f)));
	bBoxes.push_back(BoundingBox(Vector2f(1004.0f, 0.0f), Vector2f(1024.0f, 768.0f)));
}

void GameManager::loadAssetsAndBoundingBoxes(std::string arenaPath) {
	engine->createBackground(arenaPath, &prioritisedAsset, &prioritisedImageCollector);
	
	pushBackEdgeBB();
	loadLevelBoundingBox("maps/" + arenaPath + "/bb.txt", &bBoxes);
}

void GameManager::cleanUpGame() {
	delete player;
	player = 0;
	delete ball;
	ball = 0;
	bBoxes.clear();
	immobilisers.clear();
	prioritisedAsset.clear();
	gamePaused = false;
	for (Image *image : prioritisedImageCollector) {
		delete image;
	}
	prioritisedImageCollector.clear();
}

void GameManager::startGame(GameInformation gameInformation) {
	cleanUpGame();
	loadAssetsAndBoundingBoxes(gameInformation.path);

	ball = new BallObject(engine, &bBoxes);
	ball->setAtRandomPosition();
}

void GameManager::updateImmobilisers() {
	for (list<Immobiliser>::iterator it = immobilisers.begin(); it != immobilisers.end();) {
		if (it->getIsActive()) {
			it->update();
			++it;
		} else {
			it = immobilisers.erase(it);
		}
	}
}

void GameManager::drawImmobilisers() {
	for (Immobiliser &immobiliser : immobilisers) {
		immobiliser.draw();
	}
}

void GameManager::updateExplosion() {
	if (explosion->getIsActive()) {
		explosion->update();
	} else {
		engine->setOffsetCoordinates(0.0f, 0.0f);
	}
}

void GameManager::drawExplosion() {
	if (explosion->getIsActive()) {
		explosion->draw();
	}
}

void GameManager::goBackToMainScreen() {
	cleanUpGame();
	backToMainCallBack();
}

const Image *GameManager::getReady() {
	return &ready;
}