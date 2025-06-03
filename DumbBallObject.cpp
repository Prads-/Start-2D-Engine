#include "DumbBallObject.h"
#include "Explosion.h"

DumbBallObject::DumbBallObject(StartEngine *engine) : DumbObject(engine) {
	sprite.loadImage("Images/Ball.png");
	spriteIdle.loadImage("Images/BallIdle.png");
	spriteHit.loadImage("Images/BallHit.png");
	idleCircleRadius = 0.0f;
}

void DumbBallObject::setPositionPacket(DataPacketPositions::BallPositionData positionPacket) {
	if (positionPacket.flags & SERVER_BALL_EXPLODE) {
		Explosion *explosion = Explosion::getInstance();
		explosion->setPosition(Vector2f(this->positionPacket.x - 100.0f, this->positionPacket.y - 100.0f));
		explosion->setIsActive(true);
	}
	this->positionPacket = positionPacket;
}

void DumbBallObject::draw() {
	if (positionPacket.flags & SERVER_BALL_IS_TAKEN) {
		return;
	}

	if (positionPacket.flags & SERVER_BALL_IS_CATCHABLE) {
		engine->draw(&spriteIdle, 0, false, false, false, positionPacket.x, positionPacket.y);
		drawIdleVisibilityCircle();
	} else if (!(positionPacket.flags & SERVER_BALL_IS_INVISIBLE)) {
		if (positionPacket.flags & SERVER_BALL_HIT_FLAG) {
			engine->draw(&spriteHit, 0, false, false, false, positionPacket.x, positionPacket.y);
		} else {
			engine->draw(&sprite, 0, false, false, false, positionPacket.x, positionPacket.y);
		}
	}
}

void DumbBallObject::drawIdleVisibilityCircle() {
	Vector2i midPoint;
	midPoint.x = positionPacket.x + 10.0f;
	midPoint.y = positionPacket.y + 10.0f;
	engine->drawCircle(midPoint.x, midPoint.y, idleCircleRadius, 0xFF00, false);
	if (idleCircleRadius < 100.0f) {
		idleCircleRadius += 1.4f;
	} else {
		idleCircleRadius = 0.0f;
	}
}