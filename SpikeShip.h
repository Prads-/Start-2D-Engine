#ifndef SPIKE_SHIP_H
#define SPIKE_SHIP_H

#include "Enemy.h"
#include "FrameCountTimer.h"
#include "CampaignPlayer.h"

class SpikeShip : public Enemy {
private:
	enum Mode {
		MODE_NORMAL, MODE_CHASE, MODE_COOLDOWN
	}mode;

	FrameCountTimer cooldownTimer;
	int rotateSpeed;
	CampaignPlayer *player;
	const Image *deathSprite;

	void modeNormal();
	void modeChase();
	void modeCooldown();
	void checkCollisionWithPlayer();

public:
	SpikeShip(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player,
		const Image *deathSprite);

	void update();
	void draw();

	void pause();
	void resume();
};

#endif