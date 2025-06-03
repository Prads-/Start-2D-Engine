#include "E01Sensor.h"
#include "E02Rover.h"
#include <fstream>
#include "SfxPool.h"
#include "RayIntersection.h"
using namespace RayIntersection;
using namespace std;

E01Sensor::E01Sensor(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites,
		CampaignPlayer *player, E02RoverManager *e02RoverManager, std::list<EnemyManager*> *enemyManagerList, SpritePack *spritePack)
		:Enemy(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites), 
		spritePack(spritePack->spriteCooldownDetail, spritePack->spriteDeath, spritePack->spawnDetail) {
	width = 40;
	height = 40;
	radius = width / 2.0f;
	this->cooldownSprite = cooldownSprite;
	this->player = player;
	this->e02RoverManager = e02RoverManager;
	this->enemyManagerList = enemyManagerList;
	mode = MODE_NORMAL;
	cooldownTimer.changeInterval(180);
	spawnStateTimer.changeInterval(60);
	sonarRadius = 0.0f;
	this->ball = ball;
}

void E01Sensor::update() {
	if (doPreUpdate()) {
		return;
	}

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
	} else {
		if (mode == MODE_NORMAL) {
			if (!checkForObstacles(player->getPosition(), width, height)) {
				lookAt = player->getPosition() - position;
				lookAt.normalise();
				spawnE02Rovers();
			} else {
				lookAt.rotate(engine->sin(1), engine->cos(1));
				lookAt.normalise();
			}
		} else if (mode == MODE_SPAWN_STATE) {
			spawnStateMode();
		} else if (mode == MODE_COOLDOWN) {
			cooldownMode();
		}
	}
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
}

void E01Sensor::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.spriteDeath);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), 0, false, false, false, position.x, position.y);
		if (mode == MODE_COOLDOWN) {
			engine->draw(spritePack.spriteCooldownDetail, 0, false, false, false, position.x, position.y);
		} else {
			drawSensorLine();
			if (mode == MODE_SPAWN_STATE) {
				drawSpawnLines();
				const Image *spawnDetailFrame;
				spritePack.spawnDetail->getAnimationFrame(&spawnDetailFrame);
				engine->draw(spawnDetailFrame, 0, false, false, false, position.x, position.y);
			} else {
				drawSonarEffect();
			}
		}
	}

	drawStunEffect();
}

void E01Sensor::spawnE02Rovers() {
	if (e02RoverManager->isEmpty() && !checkIfManagerIsInList(e02RoverManager, enemyManagerList)) {
		enemyManagerList->push_back(e02RoverManager);
	}
	for (int i = 0; i < 3; ++i) {
		e02RoverManager->addAnEnemy(Vector2f(0.0f, 0.0f), true);
		E02Rover *e02Rover = (E02Rover*)e02RoverManager->getLastEnemy();
		e02Rover->setAtRandomPositionRelativeWithinRadius(position, 300);
		e02Rover->calculateSensorBB();
		spawnStateList.push_back(e02Rover);
	}

	vector<const Enemy*> enemies;
	for (EnemyManager *enemyManager : *enemyManagerList) {
		enemyManager->getEnemiesPointer(&enemies);
	}
	ball->setEnemies(enemies);

	mode = MODE_SPAWN_STATE;
	spawnStateTimer.startTimer();
	engine->playAudioBuffer(SfxPool::getSfxSiren1());
}

void E01Sensor::drawSpawnLines() {
	for (InteractiveObject *e02Rover : spawnStateList) {
		Vector2f midPoint = getMidPosition(), roverMidPoint = e02Rover->getMidPosition();
		Vector2f direction = roverMidPoint - midPoint;
		direction.normalise();
		float length = roverMidPoint.getDistance(midPoint);
		engine->drawLine(&midPoint, direction, length, 0x7FC800);
	}
}

void E01Sensor::spawnStateMode() {
	if (spawnStateTimer.checkTimer()) {
		spawnStateList.clear();
		mode = MODE_COOLDOWN;
		cooldownTimer.startTimer();
	}
}

void E01Sensor::cooldownMode() {
	if (cooldownTimer.checkTimer()) {
		mode = MODE_NORMAL;
	}
}

void E01Sensor::drawSensorLine() {
	Vector2f startPoint = getMidPosition();
	startPoint.x += lookAt.x * 10.0f;
	startPoint.y += lookAt.y * 10.0f;
	Line sensorLine(startPoint, Vector2f(startPoint.x + lookAt.x * 2000.0f, startPoint.y + lookAt.y * 2000.0f));
	Vector2f closestPoint = getClosestIntersectionPointAgainstObstacle(sensorLine);
	uint32 widthColors[5] = {0x9FFF0000, 0x8FFF7F00, 0x7FFFFF00, 0x8FFF7F00, 0x9FFF0000};
	engine->drawThickBeamNoBoundCheck(startPoint, lookAt, startPoint.getDistance(closestPoint), widthColors);
}

void E01Sensor::drawSonarEffect() {
	const int MAX_SONAR_RADIUS = 100;
	sonarRadius += 1;
	if (sonarRadius >= MAX_SONAR_RADIUS) {
		sonarRadius = 10;
	}
	Vector2f midPoint = getMidPosition();
	engine->drawCircle(midPoint.x, midPoint.y, sonarRadius, 0xFF00FF00, false);
}

void E01Sensor::pause() {
	Enemy::pause();
	cooldownTimer.pauseTimer();
	spawnStateTimer.pauseTimer();
}

void E01Sensor::resume() {
	Enemy::resume();
	cooldownTimer.resumeTimer();
	spawnStateTimer.resumeTimer();
}