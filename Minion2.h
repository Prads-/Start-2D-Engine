#ifndef MINION_2_H
#define MINION_2_H

#include "Minion1.h"


class Minion2 : public Minion1 {
public:
	struct SpritePack {
		Image *spriteDefaultState, *spriteAttackState, *spriteChasingState, *spriteConfusedState, *spriteHitStunnedState, *spriteConfusedEffect,
			*spriteSightDetectionEffect, *spriteDeath, *spriteAttackEffect;
	
		SpritePack(Image *spriteDefaultState, Image *spriteAttackState, Image *spriteChasingState, Image *spriteConfusedState,
				Image *spriteHitStunnedState, Image *spriteConfusedEffect,
				Image *spriteSightDetectionEffect, Image *spriteDeath, Image *spriteAttackEffect) {
			this->spriteAttackEffect = spriteAttackEffect;
			this->spriteAttackState = spriteAttackState;
			this->spriteChasingState = spriteChasingState;
			this->spriteConfusedEffect = spriteConfusedEffect;
			this->spriteConfusedState = spriteChasingState;
			this->spriteDeath = spriteDeath;
			this->spriteDefaultState = spriteDefaultState;
			this->spriteHitStunnedState = spriteHitStunnedState;
			this->spriteSightDetectionEffect = spriteSightDetectionEffect;
		}
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_CHASING, MODE_CONFUSED, MODE_HIT_STUNNED
	}mode;
	
	Vector2f lastKnownPlayerPosition;
	Image *spriteConfused, *spriteChasing;
	float chasingSpeed;
	Vector2f moveVec;
	FrameCountTimer hitStunTimer;
	SpritePack spritePack;

	void move();
	void updateMode();
	void calculateLookVector(Vector2f direction);
	void drawFacingTowardsPlayer(const Image *sprite, Vector2f faceTowards);
	
public:
	Minion2(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, int initialShadowPeriod, std::list<Immobiliser> *immobilisers, float speed, float chasingSpeed, 
		CampaignPlayer *player, Vector2f position, std::vector<Sprite*> *sprites, SpritePack *spritePack);

	void draw();
	void checkCollisionWithBall();

	void pause();
	void resume();
};

#endif