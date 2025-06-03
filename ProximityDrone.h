#ifndef PROXIMITY_DRONE_H
#define PROXIMITY_DRONE_H

#include "Enemy.h"
#include "FrameCountTimer.h"
#include "CampaignPlayer.h"
#include "Image.h"

class ProximityDrone : public Enemy {
public:
	struct SpritePack {
		Image *spriteCooldownEffect, *spriteDefaultStateEffect, *spriteExplosionStateEffect, *spriteNewAngleStateEffect, *spritePreparationStateEffect,
			*spriteDeath;

		SpritePack(Image *spriteCooldownEffect, Image *spriteDefaultStateEffect, Image *spriteExplosionStateEffect, 
				Image *spriteNewAngleStateEffect, Image *spritePreparationStateEffect, Image *spriteDeath) {
			this->spriteCooldownEffect = spriteCooldownEffect;
			this->spriteDefaultStateEffect = spriteDefaultStateEffect;
			this->spriteExplosionStateEffect = spriteExplosionStateEffect;
			this->spriteNewAngleStateEffect = spriteNewAngleStateEffect;
			this->spritePreparationStateEffect = spritePreparationStateEffect;
			this->spriteDeath = spriteDeath;
		}
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_ANGLE_CHANGE, MODE_PREPARATION, MODE_EXPLOSION, MODE_COOLDOWN
	}mode;

	int reboundCounter, randomAngle;
	float detectionDrawRadius;
	FrameCountTimer angleChangeTimer, preparationTimer, explosionTimer, coolDownTimer;
	CampaignPlayer *player;
	SpritePack spritePack;
	Animation explosion;

	void moveNormal();
	void checkCollisionWithObstacles();
	void collisionWithObstacle(const BoundingBox *bBox);
	void checkAngleChange();
	void getRandomAngle();
	void checkCollisionWithPlayer();
	void drawDetectionCircle(uint32 detectionCircleColor);
	void checkIfPlayerIsWithinTheDetectionRadius();
	bool isPlayerWithinDetectionRadius();
	void drawStunEffect();
	
public:
	ProximityDrone(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player,
		SpritePack spritePack, const Animation *explosion);

	void update();
	void draw();

	void pause();
	void resume();
};

#endif