#include "SpriteAnimation.h"

const Image *SpriteAnimation::getSprite() {
	const Image *frame;
	sprite.getAnimationFrame(&frame);
	return frame;
}

void SpriteAnimation::loadSprite(std::string path) {
	sprite.openAnimationFile(path, true);
	sprite.setLoop(true);
}