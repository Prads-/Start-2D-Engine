#ifndef SWEEPER_H
#define SWEEPER_H

#include "PathFindingEnemy.h"
#include <list>
#include <vector>
#include "Grid.h"
#include "FrameCountTimer.h"
#include "CampaignPlayer.h"
#include "Sprite.h"

#define SWEEPER_WIDTH			20
#define SWEEPER_HEIGHT			20

class Sweeper : public PathFindingEnemy {
public:
	struct SpritePack {
		Image *spriteBaseBoostEffect, *spriteChargeStateEffect, *spritePreparationStateEffect, *spriteDefaultStateEffect, *spriteDeath;

		SpritePack(Image *spriteBaseBoostEffect, Image *spriteChargeStateEffect, Image *spritePreparationStateEffect, Image *spriteDefaultStateEffect,
			Image *spriteDeath) {
			this->spriteBaseBoostEffect = spriteBaseBoostEffect;
			this->spriteChargeStateEffect = spriteChargeStateEffect;
			this->spritePreparationStateEffect = spritePreparationStateEffect;
			this->spriteDefaultStateEffect = spriteDefaultStateEffect;
			this->spriteDeath = spriteDeath;
		}
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_DASH_PREPARATION, MODE_DASH, MODE_DASH_RETRIEVE, MODE_COOLDOWN
	}mode;
	
	FrameCountTimer coolDownTimer, dashPreparationTimer;
	Vector2f dashStartPosition, dashDirection;
	CampaignPlayer *player;
	SpritePack spritePack;

	void normalMode();
	void performReturnBack();
	void drawStunEffect();

public:
	Sweeper(StartEngine *engine, Vector2f position, Image *shadowSprite,
		int initialShadowPeriod, CampaignPlayer *player, BallObject *ball, Grid **gridMap,
		const std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers, 
		std::vector<Sprite*> *sprites, SpritePack *spritePack);

	void update();
	void draw();

	void checkCollisionWithPlayer();
	void coolDown();
	
	void initiateDash();
	bool performDash();
	
	void prepareToDash();
	bool updateDashPreparation();
	void makeModeNormal();

	bool isDead() const;
	bool checkDeathTimer();

	void pause();
	void resume();
};

#endif