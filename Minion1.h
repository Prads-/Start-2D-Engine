#ifndef MINION_1_H
#define MINION_1_H

#include "Enemy.h"
#include "FrameCountTimer.h"
#include "Image.h"
#include "CampaignPlayer.h"

class Minion1 : public Enemy {
public:
	struct SpritePack {
		Image *spriteAttack, *spriteDeath;

		SpritePack(Image *spriteAttack, Image *spriteDeath) {
			this->spriteAttack = spriteAttack;
			this->spriteDeath = spriteDeath;
		}
	};

protected:
	FrameCountTimer directionChangeTimer;
	CampaignPlayer *player;
	int randomAngle;
	bool isAttacking;
	SpritePack spritePack;

	void getRandomAngle();
	void checkCollisionWithObstacles();
	void collisionWithObstacle(const BoundingBox *bBox);
	virtual void move();
	void commonInit(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriod, std::list<Immobiliser> *immobilisers, float speed, CampaignPlayer *player, 
		Vector2f position, std::vector<Sprite*> *sprites);
	void drawStunEffect();

public:
	Minion1(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriod, std::list<Immobiliser> *immobilisers, float speed, CampaignPlayer *player, 
		Vector2f position, std::vector<Sprite*> *sprites, SpritePack *spritePack);

	Minion1(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriod, std::list<Immobiliser> *immobilisers, float speed, CampaignPlayer *player, 
		Vector2f position, std::vector<Sprite*> *sprites);

	void update();
	virtual void draw();

	bool checkCollisionWithPlayer();

	virtual void pause();
	virtual void resume();
};

#endif