#include "NetworkPlayerObject.h"
#include "stringExtension.h"
using namespace StringExtension;

NetworkPlayerObject::NetworkPlayerObject(StartEngine *engine, BallObject *ball, Image *font, const std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers,
		std::string characterPath, PlayerVariables playerVariables, Image *pickupSprites, const Image *pickupCross) 
		: PlayerObject(engine, ball, font, bBoxes, immobilisers, characterPath, playerVariables, pickupSprites, pickupCross, COMPETITOR_START_HEALTH) {
	damageAnimationTimer.changeInterval(damageAnimation.getTotalTime());
}

void NetworkPlayerObject::setDataPacketClientInput(DataPacketClientInput clientInput) {
	this->clientInput = clientInput;
	if (clientInput.inputFlags & CLIENT_INPUT_C_KEY) {
		makeCursorMovementRelative ^= true;
	}
}

unsigned int NetworkPlayerObject::getId() {
	return id;
}

void NetworkPlayerObject::setId(unsigned int id) {
	this->id = id;
}

Vector2f NetworkPlayerObject::getVirtualCursorPosition() {
	return virtualCursor.getPosition();
}

void NetworkPlayerObject::takeAwayHP(int hp) {
	PlayerObject::takeAwayHP(hp);
	damageAnimationTimer.startTimer();
}

void NetworkPlayerObject::updateVirtualCursor() {
	virtualCursor.update(clientInput.mouseX, clientInput.mouseY, id == 0);
}

void NetworkPlayerObject::update() {
	if (playDamageAnimation) {
		playDamageAnimation = !damageAnimationTimer.checkTimer();
	}
	PlayerObject::update();
}

void NetworkPlayerObject::draw() {
	if (isInDeathPeriod()) {
		drawDeathPeriod(&sprite);
		return;
	}

	calculateRotateFromLook();
	drawTargetLine();

	if (_hasBall) {
		engine->draw(&spriteBall, rotateDegree, false, false, false, position.x, position.y);
		if (id == 0) {
			engine->drawText("Shoot within: " + intToStr(ballHoldSecondsLeft) + " secs", font, 32, 32, 16, 0x5252A0, 300, 5);
		}
	}

	if (playDamageAnimation) {
		const Image *frame;
		damageAnimation.getAnimationFrame(&frame);
		engine->draw(frame, rotateDegree, false, false, false, position.x, position.y);
	} else {
		if (moveState == PLAYER_MOVE_RUN) {
			engine->draw(&spriteMoving, rotateDegree, false, false, shieldActive, position.x, position.y);
		} else {
			engine->draw(&sprite, rotateDegree, false, false, shieldActive, position.x, position.y);
		}
	}

	drawStunEffect();
	drawShieldEffect();
}

bool NetworkPlayerObject::isKeyPressed(int key) {
	switch (key) {
		case INPUT_KEY_DOWN:
			return clientInput.inputFlags & CLIENT_INPUT_DOWN_KEY;
		case INPUT_KEY_UP:
			return clientInput.inputFlags & CLIENT_INPUT_UP_KEY;
		case INPUT_KEY_LEFT:
			return clientInput.inputFlags & CLIENT_INPUT_LEFT_KEY;
		case INPUT_KEY_RIGHT:
			return clientInput.inputFlags & CLIENT_INPUT_RIGHT_KEY;
		case INPUT_KEY_THROW_SPECIAL:
			return clientInput.inputFlags & CLIENT_INPUT_MID_MOUSE;
	}
	return false;
}

bool NetworkPlayerObject::wasMouseLeftToggled() {
	return clientInput.inputFlags & CLIENT_INPUT_LEFT_MOUSE;
}

bool NetworkPlayerObject::wasMouseMidToggled() {
	return clientInput.inputFlags & CLIENT_INPUT_MID_MOUSE;
}

bool NetworkPlayerObject::wasMouseRightToggled() {
	return clientInput.inputFlags & CLIENT_INPUT_RIGHT_MOUSE;
}