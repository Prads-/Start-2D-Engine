#include "Explosion.h"
#include "MT_RandomGenerator.h"
#include "SfxPool.h"

Explosion *Explosion::explosion = 0;

Explosion::Explosion(StartEngine *engine) : InteractiveObject(engine), animation(20), explosionTimer(60) {
	isActive = false;
	animation.addFrame("Images/explosion1.png");
	animation.addFrame("Images/explosion2.png");
	animation.addFrame("Images/explosion3.png");
	width = 200;
	height = 200;
	radius = 200.0f / 2.0f;
}

Explosion *Explosion::getInstance(StartEngine *engine) {
	if (!explosion) {
		explosion = new Explosion(engine);
	}
	return explosion;
}

Explosion *Explosion::getInstance() {
	return explosion;
}

void Explosion::cleanupInstance() {
	delete explosion;
	explosion = 0;
}

void Explosion::update() {
	isActive = !explosionTimer.checkTimer();
}

void Explosion::draw() {
	const Image *frame;
	animation.getAnimationFrame(&frame);
	engine->draw(frame, 0, false, false, true, position.x, position.y);
	
	int randomOffset = MT_RandomGenerator::randomNumber() % 6 - 2;
	engine->setOffsetCoordinates(randomOffset, randomOffset);
}

void Explosion::setPosition(Vector2f position) {
	this->position = position;
	calculateBB();
}

float Explosion::getRadius() {
	return radius;
}

bool Explosion::getIsActive() {
	return isActive;
}

void Explosion::setIsActive(bool isActive) {
	this->isActive = isActive;
	if (isActive) {
		animation.resetAnimation();
		explosionTimer.startTimer();
		engine->playAudioBuffer(SfxPool::getSfxExplosion1());
	}
}

Competitor *Explosion::getShooter() {
	return shooter;
}

void Explosion::setShooter(Competitor *shooter) {
	this->shooter = shooter;
}

void Explosion::pause() {
	explosionTimer.pauseTimer();
}

void Explosion::resume() {
	explosionTimer.resumeTimer();
}