#ifndef DRAGOON_H
#define DRAGOON_H

#include "PathFindingEnemy.h"
#include "CampaignPlayer.h"
#include "Image.h"
#include "FrameCountTimer.h"
#include "DragoonProjectile.h"
#include <list>

#define DRAGOON_WIDTH			40
#define DRAGOON_HEIGHT			40

class Dragoon : public PathFindingEnemy {
public:
	struct SpritePack {
		Image *spriteAttackState, *spriteDefaultState, *spriteRecoveryState, *spriteShield, *spriteDeath;
		
		SpritePack(Image *spriteAttackState, Image *spriteDefaultState, Image *spriteRecoveryState, Image *spriteShield, Image *spriteDeath) {
			this->spriteAttackState = spriteAttackState;
			this->spriteDefaultState = spriteDefaultState;
			this->spriteRecoveryState = spriteRecoveryState;
			this->spriteShield = spriteShield;
			this->spriteDeath = spriteDeath;
		}
	};

private:
	Animation shieldRecovery;
	Image *projectileSprite;
	FrameCountTimer hitStunnedTimer, coolDownTimer;
	bool isHitStunned, isCoolingDown, isShootingAtPlayer;
	std::list<DragoonProjectile> projectileList;
	Vector2f moveVec;
	const std::list<void*> *dragoons;
	bool isWaitingForOtherDragoonToMove;
	CampaignPlayer *player;
	SpritePack spritePack;

	void takeAction();
	void goStraightToPlayer();
	void shootTowardsPlayer();
	void updateCoolDownTimer();
	void updateProjectiles();
	void ballCollisionResponse();
	bool checkCollisionWithOtherDragoons();
	void checkCollision();

public:
	Dragoon(StartEngine *engine, Image *projectileSprite, Image *shadowSprite, int initialShadowPeriod, 
		const std::vector<BoundingBox> *bBoxes, BallObject *ball, Grid** gridMap, CampaignPlayer *player, Vector2f position, 
		std::list<Immobiliser> *immobilisers, const std::list<void*> *dragoons, std::vector<Sprite*> *sprites, SpritePack *spritePack,
		const Animation *shieldRecovery);
	~Dragoon();

	void update();
	void draw();

	void checkCollisionWithBall();
	bool getIsWaitingForOtherDragoonToMove() const;
	
	void pause();
	void resume();
};

#endif