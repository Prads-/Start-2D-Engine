#ifndef SPRITE_IMAGE_H
#define SPRITE_IMAGE_H

#include "Sprite.h"

class SpriteImage : public Sprite {
private:
	Image sprite;
public:
	const Image *getSprite();
	void loadSprite(std::string path);
};

#endif