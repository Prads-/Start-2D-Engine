#include "EnemyManager.h"
#include <fstream>
#include "stringExtension.h"
#include "SpriteImage.h"
#include "SpriteAnimation.h"
using namespace std;
using namespace StringExtension;

EnemyManager::EnemyManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) {
	this->engine = engine;
	this->bBoxes = bBoxes;
	this->immobilisers = immobilisers;
	killScore = 0;
}

EnemyManager::~EnemyManager() {
	makeEmpty();
	for (Sprite *sprite : sprites) {
		delete sprite;
	}
}

void EnemyManager::setPlayer(CampaignPlayer *player) {
	this->player = player;
}

void EnemyManager::setBall(BallObject *ball) {
	this->ball = ball;
}

void EnemyManager::setShadowPeriodInterval(int shadowPeriodInterval) {
	this->shadowPeriodInterval = shadowPeriodInterval;
}

void EnemyManager::updateGridMap() { }

void EnemyManager::getEnemiesPointer(std::vector<const Enemy*> *enemiesPtrOut) {
	for (void *enemy : enemies) {
		enemiesPtrOut->push_back((Enemy*)enemy);
	}
}

void EnemyManager::updateAllEnemies() {
	for (list<void*>::iterator it = enemies.begin(); it != enemies.end();) {
		((Enemy*)(*it))->update();
		if (((Enemy*)(*it))->isDead()) {
			it = enemies.erase(it);
			ball->addToScore(killScore);
		} else {
			++it;
		}
	}
}

void EnemyManager::drawAllEnemies() {
	for (void *enemy : enemies) {
		((Enemy*)enemy)->draw();
	}
}

bool EnemyManager::isEmpty() const {
	return enemies.size() == 0;
}

void EnemyManager::makeEmpty() {
	for (void *enemy : enemiesCollection) {
		delete enemy;
	}
	enemies.clear();
	enemiesCollection.clear();
}

void EnemyManager::createShadowSprite(uint8 alpha) {
	const Image *baseSprite = sprites[sprites.size() - 1]->getSprite();
	shadowSprite.createImage(baseSprite->getWidth(), baseSprite->getHeight());
	baseSprite->createShadowImage(shadowSprite.getModifiablePtr(), alpha);
}

void EnemyManager::pause() {
	for (void *enemy : enemies) {
		((Enemy*)enemy)->pause();
	}
}

void EnemyManager::resume() {
	for (void *enemy : enemies) {
		((Enemy*)enemy)->resume();
	}
}

bool EnemyManager::doesAnyEnemyCollideWithBB(const BoundingBox *bBox) const {
	for (void *enemy : enemies) {
		if (engine->checkCollision(((Enemy*)enemy)->getBoundingBox(), bBox)) {
			return true;
		}
	}
	return false;
}

void EnemyManager::loadSpriteListFile(std::string path, std::vector<Sprite*> *spritesOut) {
	std::vector<Sprite*> *modifiedList = (spritesOut) ? spritesOut : &sprites;
	ifstream spritesFile(path);
	if (spritesFile.is_open()) {
		string pathFolder = path.substr(0, path.rfind('/') + 1);
		string line;
		while (getLineFromFile(&spritesFile, &line)) {
			if (line == "0") {
				modifiedList->push_back(0);
			} else {
				Sprite *sprite;
				if (line == SPRITE_ANIMATION) {
					sprite = new SpriteAnimation();
				} else {
					sprite = new SpriteImage();
				}
				getLineFromFile(&spritesFile, &line);
				sprite->loadSprite(pathFolder + line);
				modifiedList->push_back(sprite);
			}
		}
	}
}

void *EnemyManager::getLastEnemy() {
	if (enemies.size() != 0) {
		return enemies.back();
	}
}