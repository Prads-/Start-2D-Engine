#include "MinionEgg.h"
#include "MT_RandomGenerator.h"
#include "Explosion.h"
#include <fstream>
using namespace std;

#define INITIAL_SHAKE_INTERVAL		120
#define SHAKE_DIVISOR				2.2f
#define SHAKE_DISPLACEMENT			2.0f

MinionEgg::MinionEgg(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites,
		std::list<EnemyManager*> *enemyManagers, Minion1Manager *minion1Manager, Minion2Manager *minion2Manager, Vector2f position,
		const Animation *holeOpenAnimation, const Animation *holeCloseAnimation, Animation *eggAnimation, const Image *deathSprite)
		: Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites), spawnTimer(1080),
			shakeTimer(INITIAL_SHAKE_INTERVAL), shakeUpdateTimer(216) {
	width = 60;
	height = 80;
	this->enemyManagers = enemyManagers;
	this->minion1Manager = minion1Manager;
	this->minion2Manager = minion2Manager;
	this->position = position;
	previousPosition = position;
	this->health = health;
	mode = MODE_NORMAL;
	this->holeOpenAnimation.linkAnimationFrames(holeOpenAnimation);
	this->holeOpenAnimation.changeAnimationInterval(holeOpenAnimation->getAnimationInterval());
	this->holeCloseAnimation.linkAnimationFrames(holeCloseAnimation);
	this->holeCloseAnimation.changeAnimationInterval(holeCloseAnimation->getAnimationInterval());
	this->eggAnimation = eggAnimation;
	this->deathSprite = deathSprite;
	this->ball = ball;
}

void MinionEgg::update() {
	if (doPreUpdate()) {
		return;
	}

	if (mode == MODE_NORMAL) {
		updateShake();
		if (spawnTimer.checkTimer()) {
			mode = MODE_SPAWN_PREPARE;
			holeOpenAnimation.resetAnimation();
		}
	}
	checkCollisionWithBall();
}

void MinionEgg::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(deathSprite);
	} else {
		const Image *eggAnimationFrame;
		eggAnimation->getAnimationFrame(&eggAnimationFrame);
		engine->draw(eggAnimationFrame, 0, false, false, false, position.x, position.y);
		engine->draw((*sprites)[spriteCounter]->getSprite(), 0, false, false, false, position.x, position.y);
		if (mode != MODE_NORMAL) {
			const Image *frame;

			if (mode == MODE_SPAWN_PREPARE) {
				if (holeOpenAnimation.getAnimationFrame(&frame)) {
					mode = MODE_SPAWN_END;
					spawnMinions();
					holeCloseAnimation.resetAnimation();
				}
			} else if (mode == MODE_SPAWN_END) {
				if (holeCloseAnimation.getAnimationFrame(&frame)) {
					mode = MODE_NORMAL;
					shakeTimer.changeInterval(INITIAL_SHAKE_INTERVAL);
					shakeTimer.startTimer();
					shakeUpdateTimer.startTimer();
					spawnTimer.startTimer();
				}
			}
		
			Vector2f midPosition = getMidPosition();
			engine->draw(frame, 0, false, false, false, midPosition.x - 10.0f, midPosition.y - 10.0f);
		}
	}
}

void MinionEgg::pause() {
	Enemy::pause();
	spawnTimer.pauseTimer();
	shakeTimer.pauseTimer();
	shakeUpdateTimer.pauseTimer();
	holeOpenAnimation.pause();
	holeCloseAnimation.pause();
}

void MinionEgg::resume() {
	Enemy::resume();
	spawnTimer.resumeTimer();
	shakeTimer.resumeTimer();
	shakeUpdateTimer.resumeTimer();
	holeOpenAnimation.resume();
	holeCloseAnimation.resume();
}

void MinionEgg::spawnMinions() {
	if (minion1Manager->isEmpty() && !checkIfManagerIsInList(minion1Manager, enemyManagers)) {
		enemyManagers->push_back(minion1Manager);
	}
	if (minion2Manager->isEmpty() && !checkIfManagerIsInList(minion2Manager, enemyManagers)) {
		enemyManagers->push_back(minion2Manager);
	}
	Vector2f midPosition = getMidPosition();
	midPosition.x -= 15.0f;
	midPosition.y -= 15.0f;
	for (int i = 0; i < 2; ++i) {
		minion1Manager->addAnEnemy(midPosition, false);
	}
	for (int i = 0; i < 1; ++i) {
		minion2Manager->addAnEnemy(midPosition, false);
	}

	vector<const Enemy*> enemies;
	for (EnemyManager *enemyManager : *enemyManagers) {
		enemyManager->getEnemiesPointer(&enemies);
	}
	ball->setEnemies(enemies);
}

void MinionEgg::updateShake() {
	if (shakeTimer.checkTimer()) {
		//rotateDegree = MT_RandomGenerator::randomNumber() % 360;
		//calculateLookFromRotateDegree();
		int rotation = MT_RandomGenerator::randomNumber() % 360;
		lookAt.rotate(engine->sin(rotation), engine->cos(rotation));
		setPosition(Vector2f(previousPosition.x + lookAt.x * SHAKE_DISPLACEMENT, previousPosition.y + lookAt.y * SHAKE_DISPLACEMENT));
	}
	if (shakeUpdateTimer.checkTimer()) {
		shakeTimer.changeInterval(shakeTimer.getInterval() / SHAKE_DIVISOR);
	}
}

bool MinionEgg::testCollisionWithBall() {
	return engine->checkCollision(&bBox, ball->getBoundingBox());
}

bool MinionEgg::testCollisionWithExplosion() {
	return engine->checkCollision(&bBox, Explosion::getInstance()->getBoundingBox());
}