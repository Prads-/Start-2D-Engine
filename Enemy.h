#ifndef ENEMY_OBJECT_H
#define ENEMY_OBJECT_H

#include "InteractiveObject.h"
#include "BallObject.h"
#include "Image.h"
#include "FrameCountTimer.h"
#include "Immobiliser.h"
#include "RayIntersection.h"
#include <list>
#include "Sprite.h"

class EnemyManager;

class Enemy : public InteractiveObject {
protected:
	std::vector<Sprite*> *sprites;
	int health, spriteCounter;
	BallObject *ball;
	bool isImmune;
	Image *shadowSprite;
	bool isInInitialShadowPeriod, isStunned;
	FrameCountTimer shadowPeriodTimer, stunnedTimer, deathTimer;
	std::list<Immobiliser> *immobilisers;
	float radius;
	static Animation spawnEffect, deathExplosionEffect, stunEffectAnimationFrames;
	Animation stunEffectAnimation;

	bool checkForObstacles(Vector2f to, int rightLength, int bottomLength);
	bool checkForObstacles(Vector2f to);
	void collisionResponse(const BoundingBox *bBox, Vector2f &direction);
	void checkCollisionWithImmobilisers();
	void updateStunnedPeriod();
	bool checkCollisionWithObstacles(Vector2f &direction);
	void updateSpriteCounter();
	virtual void takeAwayHP(int hp = 1);
	virtual bool doPreUpdate();
	virtual bool testCollisionWithBall();
	virtual bool testCollisionWithExplosion();
	Vector2f getClosestIntersectionPointAgainstObstacle(RayIntersection::Line line);
	bool checkIfManagerIsInList(EnemyManager *enemyManager, std::list<EnemyManager*> *enemyManagers);
	void drawShadowPeriod();
	void drawDeathPeriod(const Image *deathSprite);
	virtual void drawStunEffect();

public:
	Enemy(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites);

	bool checkShadowPeriod();
	virtual bool isDead() const;
	bool isInDeathPeriod() const;
	void updateDeathPeriod();
	bool getIsStunned() const;
	int getHealth() const;
	float getRadius() const;
	void setHealth(int health);

	virtual void checkCollisionWithBall();
	bool getIsInInitialShadowPeriod() const;

	virtual void pause();
	virtual void resume();

	static Animation *getSpawnEffect();
	static Animation *getDeathExplosionEffect();
	static Animation *getStunnedEffect();
};

#endif