#include "SpriteImage.h"

const Image *SpriteImage::getSprite() {
	return &sprite;
}

void SpriteImage::loadSprite(std::string path) {
	sprite.loadImage(path);
}