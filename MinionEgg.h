#ifndef MINION_EGG_H
#define MINION_EGG_H

#include "Enemy.h"
#include <list>
#include "EnemyManager.h"
#include "Minion1Manager.h"
#include "Minion2Manager.h"
#include "FrameCountTimer.h"
#include "Animation.h"

class MinionEgg : public Enemy {
private:
	enum Mode {
		MODE_NORMAL, MODE_SPAWN_PREPARE, MODE_SPAWN_END
	}mode;

	std::list<EnemyManager*> *enemyManagers;
	Minion1Manager *minion1Manager;
	Minion2Manager *minion2Manager;
	FrameCountTimer spawnTimer, shakeTimer, shakeUpdateTimer;
	Animation holeOpenAnimation, holeCloseAnimation, *eggAnimation;
	const Image *deathSprite;
	BallObject *ball;

	void spawnMinions();
	void updateShake();
	bool testCollisionWithBall();
	bool testCollisionWithExplosion();

public:
	MinionEgg(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites,
		std::list<EnemyManager*> *enemyManagers, Minion1Manager *minion1Manager, Minion2Manager *minion2Manager, Vector2f position,
		const Animation *holeOpenAnimation, const Animation *holeCloseAnimation, Animation *eggAnimation, const Image *deathSprite);

	void update();
	void draw();

	void pause();
	void resume();
};

#endif