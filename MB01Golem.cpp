#include "MB01Golem.h"
#include "MT_RandomGenerator.h"
#include <fstream>
using namespace std;

MB01Golem::MB01Golem(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
					 int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, 
					 Vector2f position, Grid **gridMap, std::vector<Sprite*> *sensorSprites, MB01Golem::SpritePack *spritePack, const Animation *shieldRecoveryAnimation) 
					 : PatrolRover(engine, bBoxes, health, ball, shadowSprite, initialShadowPeriodInterval, immobilisers, sprites, player, position, 
					 MB01GOLEM_WIDTH, MB01GOLEM_HEIGHT, 5),
						spritePack(*spritePack) {
	speed = 4.0f;
	artilleryShootCooldown = false;
	artilleryShootTimer.changeInterval(60);
	scatterShootTimer.changeInterval(60);
	this->gridMap = gridMap;
	mortarShootTimer.changeInterval(60);
	modeSelectTimer.changeInterval(180);
	this->sensorSprites = sensorSprites;
	shieldRechargeTimer.changeInterval(240);
	shieldRechargeDelayTimer.changeInterval(900);
	turretRotation = 0;
	mode = MODE_NORMAL;
	shieldMode = SHIELD_MODE_SHIELDED;
	this->shieldRecoveryAnimation.linkAnimationFrames(shieldRecoveryAnimation);
	this->shieldRecoveryAnimation.changeAnimationInterval(shieldRecoveryAnimation->getAnimationInterval());
	hasSeenPlayer = false;
	modeSelectTimer.changeInterval(180);
	artilleryShootTimer.changeInterval(60);
}

void MB01Golem::update() {
	if (doPreUpdate()) {
		return;
	}

	switch (mode) {
		case MODE_NORMAL:
			modeNormal();
			break;
		case MODE_ARTILLERY_SEARCH:
			modeArtillerySearch();
			break;
		case MODE_ARTILLERY_SHOOT:
			modeArtilleryShoot();
			break;
		case MODE_MORTAR:
			modeMortar();
			updateSensors();
			break;
		case MODE_MORTAR_STRIKE:
			modeMortarStrike();
			updateSensors();
			break;
		case MODE_SCATTER:
			modeScatter();
	}
	checkCollisionWithBall();
	checkCollisionWithImmobilisers();
	updateShells();
}

void MB01Golem::draw() {
	if (isInInitialShadowPeriod) {
		drawShadowPeriod();
	} else if (isInDeathPeriod()) {
		drawDeathPeriod(spritePack.deathSprite);
	} else {
		engine->draw((*sprites)[spriteCounter]->getSprite(), currentRotateDegrees, false, false, false, position.x, position.y);
		const Image *shieldSprite;
		if (shieldMode == SHIELD_MODE_SHIELDED) {
			shieldSprite = spritePack.shieldFullSprite;
		} else if (shieldMode == SHIELD_MODE_RECHARGE) {
			shieldSprite = spritePack.shieldDamageSprite;
		} else {
			shieldRecoveryAnimation.getAnimationFrame(&shieldSprite);
		}
		engine->draw(shieldSprite, 0, false, false, false, position.x - 1, position.y - 1);
		engine->draw(spritePack.turretSprite, turretRotation, false, false, false, position.x, position.y);
		for (MB01Sensor &sensor : sensors) {
			sensor.draw();
		}
		for (MB01GolemShell &shell : shells) {
			shell.draw();
		}
		if (hasSeenPlayer && mode == MODE_MORTAR_STRIKE) {
			Vector2f midPosition = getMidPosition(), playerMidPosition = player->getMidPosition();
			Vector2f lineDirection = playerMidPosition - midPosition;
			lineDirection.normalise();
			engine->drawLine(midPosition, lineDirection, midPosition.getDistance(playerMidPosition) - player->getWidth() / 2, 0xFFFF00); 
		}
	}
	/*engine->drawLine(sensorBBFront.minP, Vector2f(1.0f, 0.0f), sensorBBFront.minP.getDistance(Vector2f(sensorBBFront.maxP.x, sensorBBFront.minP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(sensorBBFront.minP, Vector2f(0.0f, 1.0f), sensorBBFront.minP.getDistance(Vector2f(sensorBBFront.minP.x, sensorBBFront.maxP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(Vector2f(sensorBBFront.minP.x, sensorBBFront.maxP.y), Vector2f(1.0f, 0.0f), Vector2f(sensorBBFront.minP.x, sensorBBFront.maxP.y).getDistance(sensorBBFront.maxP), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(sensorBBFront.maxP, Vector2f(0.0f, -1.0f), sensorBBFront.maxP.getDistance(Vector2f(sensorBBFront.maxP.x, sensorBBFront.minP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);

	engine->drawLine(sensorBBLeft.minP, Vector2f(1.0f, 0.0f), sensorBBLeft.minP.getDistance(Vector2f(sensorBBLeft.maxP.x, sensorBBLeft.minP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(sensorBBLeft.minP, Vector2f(0.0f, 1.0f), sensorBBLeft.minP.getDistance(Vector2f(sensorBBLeft.minP.x, sensorBBLeft.maxP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(Vector2f(sensorBBLeft.minP.x, sensorBBLeft.maxP.y), Vector2f(1.0f, 0.0f), Vector2f(sensorBBLeft.minP.x, sensorBBLeft.maxP.y).getDistance(sensorBBLeft.maxP), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(sensorBBLeft.maxP, Vector2f(0.0f, -1.0f), sensorBBLeft.maxP.getDistance(Vector2f(sensorBBLeft.maxP.x, sensorBBLeft.minP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);

	engine->drawLine(sensorBBRight.minP, Vector2f(1.0f, 0.0f), sensorBBRight.minP.getDistance(Vector2f(sensorBBRight.maxP.x, sensorBBRight.minP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(sensorBBRight.minP, Vector2f(0.0f, 1.0f), sensorBBRight.minP.getDistance(Vector2f(sensorBBRight.minP.x, sensorBBRight.maxP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(Vector2f(sensorBBRight.minP.x, sensorBBRight.maxP.y), Vector2f(1.0f, 0.0f), Vector2f(sensorBBRight.minP.x, sensorBBRight.maxP.y).getDistance(sensorBBRight.maxP), (changeDirection) ? 0xFF0000 : 0xFFFF00);
	engine->drawLine(sensorBBRight.maxP, Vector2f(0.0f, -1.0f), sensorBBRight.maxP.getDistance(Vector2f(sensorBBRight.maxP.x, sensorBBRight.minP.y)), (changeDirection) ? 0xFF0000 : 0xFFFF00);*/
}

void MB01Golem::modeNormal() {
	if (checkCurrentRotation()) {
		move();
		checkChangeDirection();
		if (!checkForObstacles(player->getPosition())) {
			mode = MODE_ARTILLERY_SHOOT;
			lastSeenPlayerPosition = player->getPosition();
		} else {
			checkModeSelection();
		}
	}
}

void MB01Golem::modeArtilleryShoot() {
	Vector2f direction = player->getMidPosition() - getMidPosition();
	direction.normalise();
	if (rotateTurrent(&direction)) {
		if (!artilleryShootCooldown) {
			shells.push_back(MB01GolemShell(engine, player, 30, 90, 8.0f, getMidPosition(), direction, bBoxes, 
					40, true, spritePack.artilleryExplosionAnimation, spritePack.artilleryShellAnimation));
			artilleryShootCooldown = true;
			artilleryShootTimer.startTimer();
		} else {
			artilleryShootCooldown = !artilleryShootTimer.checkTimer();
		}
	}
	if (checkForObstacles(player->getPosition())) {
		mode = MODE_ARTILLERY_SEARCH;
		findPath();
	}
}

void MB01Golem::modeArtillerySearch() {
	if (pathPoints.size() != 0) {
		if (checkCurrentRotation()) {
			Vector2f point = *pathPoints.begin();
			if (point.getDistanceSquared(position) > speed * speed) {
				setPosition(Vector2f(position.x + speed * lookAt.x, position.y + speed * lookAt.y));
			} else {
				setPosition(point);
				pathPoints.pop_front();
				if (pathPoints.size() != 0) {
					updateRotateDegrees(pathPoints.front());
				}
			}
		}
		if (!checkForObstacles(player->getPosition())) {
			mode = MODE_ARTILLERY_SHOOT;
			lastSeenPlayerPosition = player->getPosition();
		}
	} else {
		goToNormalMode();
	}
}

void MB01Golem::modeScatter() {
	if (scatterShootTimer.checkTimer() && shells.size() == 0) {
		if (scatterStrikeCounter < 3) {
			scatterShells();
			++scatterStrikeCounter;
			scatterShootTimer.startTimer();
		} else {
			goToNormalMode();
			modeSelectTimer.startTimer();
		}
	}
}

void MB01Golem::modeMortar() {
	if (sensors.size() == 0) {
		goToNormalMode();
		modeSelectTimer.startTimer();
	} else {
		for (const MB01Sensor &sensor : sensors) {
			if (sensor.getHasSeenPlayer()) {
				goToMortarStrikeMode();
				return;
			}
		}
		if (!checkForObstacles(player->getPosition())) {
			goToMortarStrikeMode();
			hasSeenPlayer = true;
		} else {
			hasSeenPlayer = false;
		}
	}
}

void MB01Golem::modeMortarStrike() {
	if (mortarShootTimer.checkTimer()) {
		if (mortarStrikeCounter < 3) {
			placeShellNearPlayer();
			++mortarStrikeCounter;
			mortarShootTimer.startTimer();
		} else {
			goToMortarMode(false);
		}
	}
	if (checkForObstacles(player->getPosition())) {
		goToMortarMode(false);
		hasSeenPlayer = false;
	} else {
		hasSeenPlayer = true;
	}
}

void MB01Golem::shieldModeShielded() {
	if (isImmune) {
		rechargeShield();
	}
}

void MB01Golem::shieldModeRecharge() {
	if (!wasImmuneInPreviousFrame && isImmune) {
		++shieldHitCounter;
		if (shieldHitCounter == 3) {
			shieldMode = SHIELD_MODE_BROKEN;
			shieldRecoveryAnimation.resetAnimation();
			shieldRechargeDelayTimer.startTimer();
		}
	} else {
		if (shieldRechargeTimer.checkTimer()) {
			shieldMode = SHIELD_MODE_SHIELDED;
		}
	}
}

void MB01Golem::shieldModeBroken() {
	if (shieldRechargeDelayTimer.checkTimer()) {
		rechargeShield();
	}
}

void MB01Golem::takeAwayHP(int hp) {
	if (shieldMode == SHIELD_MODE_BROKEN) {
		Enemy::takeAwayHP(hp);
	}
}

void MB01Golem::rechargeShield() {
	shieldMode = SHIELD_MODE_RECHARGE;
	shieldHitCounter = 0;
	shieldRechargeTimer.startTimer();
}

void MB01Golem::findPath() {
	pathPoints.clear();
	engine->findPathStraightMovesOnly(position, lastSeenPlayerPosition, gridMap, MB01GOLEM_WIDTH + 10, MB01GOLEM_HEIGHT + 10, &pathPoints);
	if (pathPoints.size() != 0) {
		updateRotateDegrees(pathPoints.front());
	}
}

void MB01Golem::updateRotateDegrees(Vector2f towards) {
	lookAt = towards - position;
	lookAt.normalise();
	calculateRotateFromLook();
}

void MB01Golem::goToNormalMode() {
	mode = MODE_NORMAL;
	calculateFacingRotateDegrees();
	calculateLook();
	calculateSensorBB();
}

void MB01Golem::goToScatterMode() {
	mode = MODE_SCATTER;
	scatterStrikeCounter = 0;
}

void MB01Golem::goToMortarMode(bool instantiateSensors) {
	mode = MODE_MORTAR;
	if (instantiateSensors) {
		for (int i = 0; i < 3; ++i) {
			sensors.push_back(MB01Sensor(engine, bBoxes, 1, ball, 0, 0, immobilisers, sensorSprites, player, getMidPosition(), spritePack.sensorDeathSprite));
		}
	}
}

void MB01Golem::goToMortarStrikeMode() {
	mortarStrikeCounter = 0;
	mode = MODE_MORTAR_STRIKE;
}

void MB01Golem::scatterShells() {
	for (int i = 0; i < 5; ++i) {
		MB01GolemShell shell(engine, player, 30, 60, 0.0f, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), bBoxes, 
			80, false, spritePack.mortarExplosionAnimation, spritePack.mortarFallAnimation);
		do {
			shell.setAtRandomPosition();
		} while (!checkScatteredShellPosition(&shell));
		shells.push_back(shell);
	}
}

bool MB01Golem::checkScatteredShellPosition(MB01GolemShell *shell) {
	int explosionRadius = shell->getExplosionRadius();

	for (const MB01GolemShell &shellInList : shells) {
		if (shell->getPosition().getDistance(shellInList.getPosition()) <= explosionRadius) {
			return false;
		}
	}

	return true;
}

void MB01Golem::placeShellNearPlayer() {
	MB01GolemShell shell(engine, player, 30, 60, 0.0f, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), 
		bBoxes, 80, false, spritePack.mortarExplosionAnimation, spritePack.mortarFallAnimation);
	shell.setAtRandomPositionRelativeWithinRadius(player->getPosition(), 70);
	shells.push_back(shell);
}

void MB01Golem::checkModeSelection() {
	if (modeSelectTimer.checkTimer()) {
		int randomMode = MT_RandomGenerator::randomNumber() % 5;
		if (randomMode < 4) {
			goToMortarMode(true);
		} else {
			goToScatterMode();
		}
	}
}


void MB01Golem::checkCollisionWithBall() {
	Enemy::checkCollisionWithBall();
	switch (shieldMode) {
		case SHIELD_MODE_SHIELDED:
			shieldModeShielded();
			break;
		case SHIELD_MODE_RECHARGE:
			shieldModeRecharge();
			break;
		case SHIELD_MODE_BROKEN:
			shieldModeBroken();
	}
	wasImmuneInPreviousFrame = isImmune;
}

void MB01Golem::updateSensors() {
	for (list<MB01Sensor>::iterator it = sensors.begin(); it != sensors.end();) {
		if (it->isDead()) {
			it = sensors.erase(it);
		} else {
			it->update();
			++it;
		}
	}
}

void MB01Golem::updateShells() {
	for (list<MB01GolemShell>::iterator it = shells.begin(); it != shells.end();) {
		if (it->isActive()) {
			it->update();
			++it;
		} else {
			it = shells.erase(it);
		}
	}
}

bool MB01Golem::rotateTurrent(const Vector2f *direction) {
	Vector2f turrentDirection = Vector2f(0.0f, -1.0f).getRotate(engine->sin(turretRotation), engine->cos(turretRotation));
	int angle = direction->getSignedAngleBetweenInDegrees(turrentDirection);
	if (angle < 180) {
		if (angle < 7) {
			turretRotation += angle;
			if (turretRotation >= 360) {
				turretRotation -= 360;
			}
			return true;
		} else {
			turretRotation += 7;
			if (turretRotation >= 360) {
				turretRotation -= 360;
			}
		}
	} else {
		angle = 360 - angle;
		if (angle < 7) {
			turretRotation -= angle;
			if (turretRotation <= 0) {
				turretRotation = 360 - turretRotation;
			}
			return true;
		} else {
			turretRotation -= 7;
			if (turretRotation <= 0) {
				turretRotation = 360 - turretRotation;
			}
		}
	}
	if (angle - 5 <= 5) {
		return true;
	} else {
		return false;
	}
}

void MB01Golem::pause() {
	PatrolRover::pause();
	artilleryShootTimer.pauseTimer();
	scatterShootTimer.pauseTimer();
	mortarShootTimer.pauseTimer();
	modeSelectTimer.pauseTimer();
	shieldRechargeTimer.pauseTimer();
	shieldRechargeDelayTimer.pauseTimer();
}

void MB01Golem::resume() {
	PatrolRover::resume();
	artilleryShootTimer.resumeTimer();
	scatterShootTimer.resumeTimer();
	mortarShootTimer.resumeTimer();
	modeSelectTimer.resumeTimer();
	shieldRechargeTimer.resumeTimer();
	shieldRechargeDelayTimer.resumeTimer();
}

bool MB01Golem::doPreUpdate() {
	if (checkShadowPeriod()) {
		modeSelectTimer.startTimer();
		return true;
	} else if (isInDeathPeriod()) {
		updateDeathPeriod();
		return true;
	}
	return false;
}