#ifndef SPRITE_H
#define SPRITE_H

#include "Image.h"
#include <string>

#define SPRITE_ANIMATION		"1"
#define SPRITE_IMAGE			"2"

class Sprite {
public:
	virtual const Image *getSprite() = 0;
	virtual void loadSprite(std::string path) = 0;
};

#endif