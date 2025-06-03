#ifndef ENEMY_SPRITE_H
#define ENEMY_SPRITE_H

#include <vector>
#include "Sprite.h"

struct EnemySprites {
	int currentSpriteCounter;
	std::vector<Sprite*> sprites; 

	EnemySprites() {
		currentSpriteCounter = 0;
	}

	~EnemySprites() {
		for (Sprite *sprite : sprites) {
			delete sprite;
		}
	}
};

#endif