#ifndef SPRITE_ANIMATION_H
#define SPRITE_ANIMATION_H

#include "Sprite.h"
#include "Animation.h"

class SpriteAnimation : public Sprite {
private:
	Animation sprite;

public:
	const Image *getSprite();
	void loadSprite(std::string path);
};

#endif