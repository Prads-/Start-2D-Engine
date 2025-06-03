#include "PlayerObject.h"
#include <string>
#include "stringExtension.h"
#include "AIObject.h"
#include "SfxPool.h"
using namespace std;
using namespace StringExtension;

PlayerObject::PlayerObject(StartEngine *engine, BallObject *ball, Image *font, const std::vector<BoundingBox> *bBoxes, 
		std::list<Immobiliser> *immobilisers, std::string characterPath, PlayerVariables playerVariables, Image *pickupSprites,
		const Image *pickupCross, int health) : 
		Competitor(engine, ball, immobilisers, bBoxes, characterPath, (PickupMove)playerVariables.specialThrow, 
			(PickupMove)playerVariables.secondaryAbility, health), 
		ballHoldSecondsTimer(60), virtualCursor(engine), hud(engine, &specialMoveCooldownTimer, &secondaryAbilityCooldownTimer,
		&specialMoveCooldown, &secondaryAbilityCooldown, &_hasBall, &specialMove, &secondaryAbility, &pickupMove, pickupSprites, pickupCross)  {
	_hasBall = false;

	width = playerVariables.width;
	height = playerVariables.height;
	radius = width / 2.0f;
	maxSpeed = playerVariables.maxSpeed;
	this->pickupSprites = pickupSprites;
	
	ballHoldSecondsLeft = 0;
	this->font = font;
	moveState = PLAYER_MOVE_RUN;
	maxHealth = health;
	makeHealthBarFull();
	renderHUD();
	makeCursorMovementRelative = true;
}

void PlayerObject::move() {
	if (isKeyPressed(INPUT_KEY_UP)) {
		if (isKeyPressed(INPUT_KEY_RIGHT)) {
			moveVec = Vector2f(0.7071f, -0.7071f);
		} else if (isKeyPressed(INPUT_KEY_LEFT)) {
			moveVec = Vector2f(-0.7071f, -0.7071f);
		} else {
			moveVec = Vector2f(0.0f, -1.0f);
		}
		moveState = PLAYER_MOVE_RUN;
		speed = maxSpeed;
	} else if (isKeyPressed(INPUT_KEY_RIGHT)) {
		if (isKeyPressed(INPUT_KEY_DOWN)) {
			moveVec = Vector2f(0.7071f, 0.7071f);
		} else if (engine->isKeyPressed(INPUT_KEY_UP)) {
			moveVec = Vector2f(0.7071f, -0.7071f);
		} else {
			moveVec = Vector2f(1.0f, 0.0f);
		}
		moveState = PLAYER_MOVE_RUN;
		speed = maxSpeed;
	} else if (isKeyPressed(INPUT_KEY_DOWN)) {
		if (isKeyPressed(INPUT_KEY_LEFT)) {
			moveVec = Vector2f(-0.7071f, 0.7071f);
		} else if (engine->isKeyPressed(INPUT_KEY_RIGHT)) {
			moveVec = Vector2f(0.7071f, 0.7071f);
		} else {
			moveVec = Vector2f(0.0f, 1.0f);
		}
		moveState = PLAYER_MOVE_RUN;
		speed = maxSpeed;
	} else if (isKeyPressed(INPUT_KEY_LEFT)) {
		if (isKeyPressed(INPUT_KEY_UP)) {
			moveVec = Vector2f(-0.7071f, -0.7071f);
		} else if (engine->isKeyPressed(INPUT_KEY_DOWN)) {
			moveVec = Vector2f(-0.7071f, 0.7071f);
		} else {
			moveVec = Vector2f(-1.0f, 0.0f);
		}
		moveState = PLAYER_MOVE_RUN;
		speed = maxSpeed;
	} else {
		moveState = PLAYER_MOVE_STAND;
		speed = 0.0f;
	}
}

void PlayerObject::rotate() {
	lookAt = virtualCursor.getPosition() - getMidPosition();
	lookAt.normalise();
}

void PlayerObject::checkMoveState() {
	if (moveState == PLAYER_MOVE_RUN) {
		position.x += moveVec.x * ((batterySpeedUp) ? speed * BATTERY_SPEED_UP : speed);
		position.y += moveVec.y * ((batterySpeedUp) ? speed * BATTERY_SPEED_UP : speed);
	}
}

void PlayerObject::collisionWithObjects(const BoundingBox *bBox) {
	CollisionSide collisionSide = getCollisionSide(bBox);
	switch (collisionSide) {
		case TOP:
			if (moveVec.y < 0.0f) {
				moveVec.y = 0.0f;
				position.y = previousPosition.y;
				calculateBB();
			}
			break;
		case BOTTOM:
			if (moveVec.y > 0.0f) {
				moveVec.y = 0.0f;
				position.y = previousPosition.y;
				calculateBB();
			}
			break;
		case RIGHT:
			if (moveVec.x > 0.0f) {
				moveVec.x = 0.0f;
				position.x = previousPosition.x;
				calculateBB();
			}
			break;
		case LEFT:
			if (moveVec.x < 0.0f) {
				moveVec.x = 0.0f;
				position.x = previousPosition.x;
				calculateBB();
			}
	}
}

void PlayerObject::checkBallThrow() {
	if (_hasBall) {
		if (wasMouseLeftToggled()) {
			shoot(PICKUP_NONE);
			renderHUD();
		} else if (isKeyPressed(INPUT_KEY_THROW_SPECIAL)) {
			if (!specialMoveCooldown) {
				shoot(specialMove);
				specialMoveCooldown = true;
				specialMoveCooldownTimer.startTimer();
				renderHUD();
			}
		} else if (wasMouseRightToggled()) {
			if (checkSpecialThrow(pickupMove)) {
				takeAwayPickupMove();
			}
		} else {
			checkBallHoldTimer();
		}
	} else {
		if (wasMouseLeftToggled()) {
			if (!secondaryAbilityCooldown) {
				if (checkSecondaryAbility(secondaryAbility)) {
					secondaryAbilityCooldown = true;
					secondaryAbilityCooldownTimer.startTimer();
				}
			}
		} else if (wasMouseRightToggled()) {
			if (checkSecondaryAbility(pickupMove)) {
				takeAwayPickupMove();
			}
		}
	}
}

bool PlayerObject::checkSpecialThrow(PickupMove specialThrow) {
	switch(specialThrow) {
		case PICKUP_GRANADE:
		case PICKUP_HOMING:
		case PICKUP_PINBALL:
		case PICKUP_PIERCING_BALL:
		case PICKUP_INVISIBLE_BALL:
			shoot(specialThrow);
			return true;
		default:
			checkBallHoldTimer();
	}
	return false;
}

bool PlayerObject::checkSecondaryAbility(PickupMove secondaryAbility) {
	switch (secondaryAbility) {
		case PICKUP_IMMOBILISER:
			shootImmobiliser();
			break;
		case PICKUP_SHIELD:
			activateShield();
			break;
		case PICKUP_BATTERY:
			activateBatterySpeedUp();
			break;
		case PICKUP_TELEPORT_BALL:
			teleportBall();
			break;
		case PICKUP_CALL_BALL:
			return callBall();
		default:
			return false;
	}
	return true;
}

bool PlayerObject::checkBallHoldTimer() {
	if (!Competitor::checkBallHoldTimer()) {
		if (ballHoldSecondsTimer.checkTimer()) {
			--ballHoldSecondsLeft;
		}
		return false;
	}
	return true;
}

void PlayerObject::shoot(PickupMove move) {
	ball->setLook(lookAt);
	Vector2f ballPosition = getMidPosition();
	ballPosition.x -= BALL_WIDTH / 2;
	ballPosition.y -= BALL_HEIGHT / 2;
	ball->setPosition(ballPosition);
	ball->resetPreviousPosition();
	ball->setAtShootSpeed();
	ball->setTaken(false);
	ball->setShooter(this);
	ball->setBallType((BallType)move);
	_hasBall = false;
	isImmune = true;
	shotBall = true;
	playBallShootSfx();
}

void PlayerObject::collisionWithBall() {
	Competitor::collisionWithBall();
	if (ball->getSpeed() < BALL_CATCHABLE_SPEED)  {
		ballHoldSecondsLeft = (difficulty == AI_EASY) ? 15 : 10;
		ballHoldSecondsTimer.startTimer();
	}
}

void PlayerObject::update() {
	if (isInDeathPeriod()) {
		updateDeathPeriod();
		return;
	}

	shotImmobiliser = false;
	shotBall = false;

	updateVirtualCursor();
	updateImmunity();

	if (isStunned) {
		isStunned = !stunnedTimer.checkTimer();
		checkCollisionWithBall();
		checkCollisionWithImmobilisers();
		if (_hasBall) {
			checkBallHoldTimer();
		}
	} else {
		previousPosition = position;
		move();
		rotate();
		checkMoveState();
		calculateBB();

		checkCollision();
		
		checkBallThrow();
		if (makeCursorMovementRelative) {
			updateCursorPosition();
		}
	}
	updateShield();
	updateBatterySpeedUp();
	updateCooldownPeriod();
	checkCursorMovementToggle();
}

void PlayerObject::checkCursorMovementToggle() {
	if (engine->wasKeyToggled(INPUT_KEY_CURSOR_TOGGLE)) {
		makeCursorMovementRelative ^= true;
	}
}

void PlayerObject::draw() {
	if (isInDeathPeriod()) {
		drawDeathPeriod(&sprite);
		return;
	}

	calculateRotateFromLook();
	drawTargetLine();

	if (_hasBall) {
		engine->draw(&spriteBall, rotateDegree, false, false, false, position.x, position.y);
		engine->drawText("Shoot within: " + intToStr(ballHoldSecondsLeft) + " secs", font, 32, 32, 16, 0x5252A0, 300, 5);
	}

	if (playDamageAnimation) {
		const Image *frame;
		if (damageAnimation.getAnimationFrame(&frame)) {
			playDamageAnimation = false;
		}
		engine->draw(frame, rotateDegree, false, false, false, position.x, position.y);
	} else {
		if (moveState == PLAYER_MOVE_RUN) {
			engine->draw(&spriteMoving, rotateDegree, false, false, false, position.x, position.y);
		} else {
			engine->draw(&sprite, rotateDegree, false, false, false, position.x, position.y);
		}
	}

	drawStunEffect();
	drawShieldEffect();
}

void PlayerObject::presentHUD() {
	if (hudWasUpdated) {
		engine->presentHUD();
		hudWasUpdated = false;
	}
}

bool PlayerObject::isMoving() const {
	return (speed == 0.0f) ? false : true;
}

void PlayerObject::pause() {
	Competitor::pause();
	ballHoldSecondsTimer.pauseTimer();
}

void PlayerObject::resume() {
	Competitor::resume();
	ballHoldSecondsTimer.resumeTimer();
}

void PlayerObject::drawTargetLine() {
	Vector2f startPoint = getMidPosition();
	
	engine->drawLine(&startPoint, lookAt, 220, 0xFF0000);
	engine->drawLineFade(startPoint, lookAt, 200, 0xFF0000);
}

void PlayerObject::makeHealthBarFull() {
	hud.makeHealthBarFull();
}

void PlayerObject::renderHUD() {
	hud.renderHUD();
	hudWasUpdated = true;
}

void PlayerObject::updateHealthBar() {
	hud.updateHealthBar(health, maxHealth);
	renderHUD();
}

void PlayerObject::takeAwayHP(int hp) {
	Competitor::takeAwayHP(hp);
	updateHealthBar();
}

void PlayerObject::takeAwayPickupMove() {
	pickupMove = PICKUP_NONE;
	renderHUD();
}

bool PlayerObject::isKeyPressed(int key) {
	return engine->isKeyPressed(key);
}

bool PlayerObject::wasMouseLeftToggled() {
	return engine->wasMouseLeftToggled();
}

bool PlayerObject::wasMouseMidToggled() {
	return engine->wasMouseMiddleToggled();
}

bool PlayerObject::wasMouseRightToggled() {
	return engine->wasMouseRightToggled();
}

void PlayerObject::updateCursorPosition() {
	Vector2f cursorPosition = virtualCursor.getPosition(),
		direction = position - previousPosition;
	virtualCursor.setPosition(Vector2f(cursorPosition.x + direction.x, cursorPosition.y + direction.y));
}

void PlayerObject::updateVirtualCursor() {
	uint16 mouseX, mouseY;
	engine->getMouseXY(mouseX, mouseY);
	virtualCursor.update(mouseX, mouseY);
}

void PlayerObject::drawCursor() {
	virtualCursor.draw();
}

void PlayerObject::setAIDifficulty(AIDifficulty difficulty) {
	this->difficulty = difficulty;
	if (difficulty == AI_EASY) {
		ballHoldTimer.changeInterval(900);
	}
}

void PlayerObject::playBallShootSfx() {
	engine->playAudioBuffer(SfxPool::getSfxBallShoot());
}

void PlayerObject::playHurtSfx() {
	engine->playAudioBuffer(SfxPool::getSfxPlayerHurt());
}

void PlayerObject::playDeathSfx() {
	engine->playAudioBuffer(SfxPool::getSfxPlayerDeath());
}