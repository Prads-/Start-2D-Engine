#include "Immobiliser.h"

#define IMMOBILISER_SPEED			10.0f

Animation Immobiliser::animation(6, true);

Immobiliser::Immobiliser(StartEngine *engine) : InteractiveObject(engine) {
	isActive = true;
	engine->getFrameSize(frameWidth, frameHeight);
	width = 18;
	height = 30;
}

void Immobiliser::addAnimationFrame(std::string framePath) {
	animation.addFrame(framePath);
}

bool Immobiliser::getIsActive() {
	return isActive;
}

void Immobiliser::update() {
	position.x += lookAt.x * IMMOBILISER_SPEED;
	position.y += lookAt.y * IMMOBILISER_SPEED;
	calculateBB();
	if (position.x < 0 || position.x > frameWidth || position.y < 0 || position.y > frameHeight) {
		isActive = false;
	}
}

void Immobiliser::draw() {
	const Image *frame;
	animation.getAnimationFrame(&frame);
	engine->draw(frame, rotateDegree, false, false, false, position.x, position.y);
}

const Competitor *Immobiliser::getThrower() const {
	return thrower;
}

void Immobiliser::setThrower(Competitor *thrower) {
	this->thrower = thrower;
}