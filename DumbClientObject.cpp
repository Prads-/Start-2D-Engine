#include "DumbClientObject.h"
#include <fstream>
#include <string>
#include "stringExtension.h"
#include "FileLoader.h"
#include "AbilityCoolDownValues.h"
#include "SfxPool.h"
#include "Enemy.h"
#include "Competitor.h"
using namespace std;
using namespace StringExtension;
using namespace FileLoader;

#define SHIP_SIZE			40.0f

DumbClientObject::DumbClientObject(StartEngine *engine, int characterId, Image *font, Image *cursor, Image *shootFrame, Image *pickupSprites, Image *pickupCross, std::list<Immobiliser> *immobilisers) 
				: DumbObject(engine), secondTimer(60), hud(engine, &specialAttackCooldownTimer, &secondaryMoveCooldownTimer, &specialAttackCooldown,
				&secondaryMoveCooldown, &hasBall, &specialAttack, &secondaryMove, &pickupMove, pickupSprites, pickupCross), deathPeriodTimer(90) {
	vector<string> characterList;
	loadCharacterList(&characterList);

	string characterPath;
	try {
		characterPath = characterList.at(characterId);
	} catch (out_of_range) {
		characterPath = characterList[0];
	}
	sprite.loadImage("characters/" + characterPath + "/sprite.png");
	spriteMoving.loadImage("characters/" + characterPath + "/moving.png");
	spriteBall.loadImage("characters/" + characterPath + "/ball.png");
	damageAnimation.openAnimationFile("characters/" + characterPath + "/damage.anim");
	this->font = font;
	this->cursor = cursor;
	this->shootFrame = shootFrame;
	hasBall = false;
	specialAttackCooldown = false;
	secondaryMoveCooldown = false;
	Character character(characterPath, false);
	specialAttack = (PickupMove)character.getPlayerVariables().specialThrow;
	secondaryMove = (PickupMove)character.getPlayerVariables().secondaryAbility;
	AbilityCoolDownValue abilityCooldownValue;
	specialAttackCooldownTimer.changeInterval(abilityCooldownValue.getCooldownValue(specialAttack));
	secondaryMoveCooldownTimer.changeInterval(abilityCooldownValue.getCooldownValue(secondaryMove));
	pickupMove = PICKUP_NONE;
	newPositionPacket = false;
	hudNeedsUpdate = false;
	hud.makeHealthBarFull();
	this->pickupSprites = pickupSprites;
	this->pickupCross = pickupCross;
	this->immobilisers = immobilisers;
	wasDamagedInPreviousFrame = false;
	dead = false;
	inDeathPeriod = false;
	width = sprite.getWidth();
	height = sprite.getHeight();
	wasStunnedInPreviousFrame = false;
	wasShieldedInPreviousFrame = false;

	stunEffectAnimation.linkAnimationFrames(Enemy::getStunnedEffect());
	stunEffectAnimation.changeAnimationInterval(180 / stunEffectAnimation.getNumberOfFrames());
	stunEffectAnimation.setLoop(false);
	shieldEffectAnimation.linkAnimationFrames(Competitor::getShieldEffectAnimation());
	shieldEffectAnimation.changeAnimationInterval(240 / shieldEffectAnimation.getNumberOfFrames());
	shieldEffectAnimation.setLoop(false);
}

void DumbClientObject::setPositionPacket(DataPacketPositions::OpponentPositionData positionPacket, DataPacketPositions::MiscPosition miscPosition) {
	previousPosition = Vector2i(this->positionPacket.x, this->positionPacket.y);
	if (positionPacket.flags & SERVER_PLAYER_MY_SHIP) {
		if (!(this->positionPacket.flags & SERVER_PLAYER_HAS_BALL) && (positionPacket.flags & SERVER_PLAYER_HAS_BALL)) {
			ballHoldSecondsLeft = 10;
			secondTimer.startTimer();
		}
		this->miscPosition = miscPosition;
	}
	if (positionPacket.flags & SERVER_PLAYER_IS_DEAD && !(this->positionPacket.flags & SERVER_PLAYER_IS_DEAD)) {
		inDeathPeriod = true;
		deathPeriodTimer.startTimer();
	}
	this->positionPacket = positionPacket;
	newPositionPacket = true;
}

void DumbClientObject::update() {
	if (inDeathPeriod) {
		dead = deathPeriodTimer.checkTimer();
	} else {
		processCommonPositionPacket();
		if (positionPacket.flags & SERVER_PLAYER_MY_SHIP) {
			if (this->positionPacket.flags & SERVER_PLAYER_HAS_BALL && secondTimer.checkTimer()) {
				--ballHoldSecondsLeft;
			}
			processMyPositionPacket();
			updateHUD();
		}
	}
}

void DumbClientObject::draw() {
	drawTargetLine();
	drawCursor();

	if (positionPacket.flags & SERVER_PLAYER_HAS_BALL) {
		engine->draw(&spriteBall, positionPacket.rotateDegree, false, false, false, positionPacket.x, positionPacket.y);
		if (positionPacket.flags & SERVER_PLAYER_MY_SHIP) {
			engine->draw(shootFrame, 0, false, false, true, 287, 0);
			engine->drawText("Shoot within: " + intToStr(ballHoldSecondsLeft) + " secs", font, 32, 32, 16, 0x5252A0, 300, 5);
		}
	}

	if (inDeathPeriod) {
		engine->draw(&sprite, positionPacket.rotateDegree, false, false, false, positionPacket.x, positionPacket.y);
		const Image *explosionFrame;
		Enemy::getDeathExplosionEffect()->getAnimationFrame(&explosionFrame);
		Vector2i midPosition = getMidPosition();
		engine->draw(explosionFrame, 0, false, false, true, midPosition.x - explosionFrame->getWidth() / 2, midPosition.y - explosionFrame->getHeight() / 2);
	} else if (positionPacket.flags & SERVER_PLAYER_IS_DAMAGED) {
		const Image *frame;
		damageAnimation.getAnimationFrame(&frame);
		engine->draw(frame, positionPacket.rotateDegree, false, false, false, positionPacket.x, positionPacket.y);
		drawStunnedAndShieldPeriod();
	} else {
		if (positionPacket.x != previousPosition.x || positionPacket.y != previousPosition.y) {
			engine->draw(&spriteMoving, positionPacket.rotateDegree, false, false, false, positionPacket.x, positionPacket.y);
		} else {
			engine->draw(&sprite, positionPacket.rotateDegree, false, false, false, positionPacket.x, positionPacket.y);
		}
		drawStunnedAndShieldPeriod();
	}
}

bool DumbClientObject::isDead() {
	return dead;
}

bool DumbClientObject::isMe() {
	return positionPacket.flags & SERVER_PLAYER_MY_SHIP;
}

Vector2i DumbClientObject::getMidPosition() {
	return Vector2i(positionPacket.x + width / 2, positionPacket.y + height / 2);
}

void DumbClientObject::drawTargetLine() {
	if (positionPacket.flags & SERVER_PLAYER_MY_SHIP) {
		Vector2f midPoint;
		midPoint.x = positionPacket.x + SHIP_SIZE / 2.0f;
		midPoint.y = positionPacket.y + SHIP_SIZE / 2.0f;
		
		Vector2f lookAt = Vector2f(0.0f, -1.0f).getRotate(engine->sin(positionPacket.rotateDegree), engine->cos(positionPacket.rotateDegree));
		lookAt.normalise();

		engine->drawLine(&midPoint, lookAt, 220, 0xFF0000);
		engine->drawLineFade(midPoint, lookAt, 200, 0xFF0000);
	}
}

void DumbClientObject::drawCursor() {
	if (positionPacket.flags & SERVER_PLAYER_MY_SHIP) {
		engine->draw(cursor, 0, false, false, false, miscPosition.vMouseX - cursor->getWidth() / 2, miscPosition.vMouseY - cursor->getHeight() / 2);
	}
}

void DumbClientObject::updateHUD() {
	if (hudNeedsUpdate || specialAttackCooldown || secondaryMoveCooldown) {
		hudNeedsUpdate = false;
		hud.renderHUD();
		engine->presentHUD();
	}
}

void DumbClientObject::processMyPositionPacket() {
	if (newPositionPacket) {
		newPositionPacket = false;
		if (health != (miscPosition.flags & SERVER_PLAYER_HEALTH)) {
			health = miscPosition.flags & SERVER_PLAYER_HEALTH;
			hud.updateHealthBar(health, 10);
			hudNeedsUpdate = true;
		}
		if (positionPacket.flags & SERVER_PLAYER_HAS_BALL) {
			if (!hasBall) {
				hudNeedsUpdate = true;
				hasBall = true;
			}
		} else {
			if (hasBall) {
				hudNeedsUpdate = true;
				hasBall = false;
			}
		}
		
		if (pickupMove != (PickupMove)miscPosition.pickup) {
			pickupMove = (PickupMove)miscPosition.pickup;
			hudNeedsUpdate = true;
		}
		if (miscPosition.flags & SERVER_SPECIAL_COOLDOWN) {
			if (!specialAttackCooldown) {
				specialAttackCooldownTimer.startTimer();
				specialAttackCooldown = true;
			}
		} else {
			specialAttackCooldown = false;
		}
		if (miscPosition.flags & SERVER_SECONDARY_COOLDOWN) {
			if (!secondaryMoveCooldown) {
				secondaryMoveCooldownTimer.startTimer();
			}
			secondaryMoveCooldown = true;
		} else {
			secondaryMoveCooldown = false;
		}
	}
}

void DumbClientObject::processCommonPositionPacket() {
	if (newPositionPacket) {
		if (!(positionPacket.flags & SERVER_PLAYER_MY_SHIP)) {
			newPositionPacket = false;
		}
		if (positionPacket.flags & SERVER_PLAYER_HAS_SHOT_BALL) {
			engine->playAudioBuffer(SfxPool::getSfxBallShoot());
		}
		if (positionPacket.flags & SERVER_PLAYER_IS_DAMAGED) {
			if (!wasDamagedInPreviousFrame) {
				wasDamagedInPreviousFrame = true;
				engine->playAudioBuffer(SfxPool::getSfxPlayerHurt());
			}
		} else {
			wasDamagedInPreviousFrame = false;
		}
		if (positionPacket.flags & SERVER_PLAYER_HAS_SHOT_STUN) {
			Immobiliser immobiliser(engine);
			immobiliser.setStartLook(Vector2f(0.0f, -1.0f));
			immobiliser.setLook(Vector2f(0.0f, -1.0f).getRotate(engine->sin(positionPacket.rotateDegree), engine->cos(positionPacket.rotateDegree)));
			immobiliser.setPosition(Vector2f(positionPacket.x, positionPacket.y));
			immobilisers->push_back(immobiliser);
		}
		if (positionPacket.flags & SERVER_PLAYER_IS_STUNNED) {
			if (!wasStunnedInPreviousFrame) {
				wasStunnedInPreviousFrame = true;
				stunEffectAnimation.resetAnimation();
			}
		} else {
			wasStunnedInPreviousFrame = false;
		}
		if (positionPacket.flags & SERVER_PLAYER_IS_SHIELDED) {
			if (!wasShieldedInPreviousFrame) {
				wasShieldedInPreviousFrame = true;
				shieldEffectAnimation.resetAnimation();
			}
		} else {
			wasShieldedInPreviousFrame = false;
		}
	}
}

void DumbClientObject::drawStunnedAndShieldPeriod() {
	if (positionPacket.flags & SERVER_PLAYER_IS_STUNNED) {
		const Image *frame;
		stunEffectAnimation.getAnimationFrame(&frame);
		engine->draw(frame, 0, false, false, false, positionPacket.x - 3, positionPacket.y - 3);
	}
	if (positionPacket.flags & SERVER_PLAYER_IS_SHIELDED) {
		const Image *frame;
		shieldEffectAnimation.getAnimationFrame(&frame);
		engine->draw(frame, 0, false, false, false, positionPacket.x - 3, positionPacket.y - 3);
	}
}