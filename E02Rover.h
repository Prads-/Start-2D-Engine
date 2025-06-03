#ifndef E02ROVER_H
#define E02ROVER_H

#include "PatrolRover.h"

class E02Rover : public PatrolRover {
public:
	struct SpritePack {
		const Image *spritePreparationEffect, *spriteAttackEffect, *spriteDeath, *spriteShield;
	
		SpritePack(const Image *spritePreparationEffect, const Image *spriteAttackEffect, const Image *spriteDeath, const Image *spriteShield) {
			this->spritePreparationEffect = spritePreparationEffect;
			this->spriteAttackEffect = spriteAttackEffect;
			this->spriteDeath = spriteDeath;
			this->spriteShield = spriteShield;
		}
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_ATTACK, MODE_TELEGRAPH, MODE_RECOVERY
	}mode;

	FrameCountTimer telegraphTimer, attackTimer, recoverTimer;
	SpritePack spritePack;
	Animation shieldRecovery;

	void normalMode();
	void telegraphMode();
	void attackMode();
	void recoverMode();

	void checkCollisionWithPlayer();
	void attackToNormalMode();

public:
	E02Rover(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, 
		Vector2f position, const SpritePack *spritePack, const Animation *shieldRecovery);

	void update();
	void draw();

	void checkCollisionWithBall();

	void pause();
	void resume();
};

#endif