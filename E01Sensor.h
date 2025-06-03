#ifndef E01SENSOR_H
#define E01SENSOR_H

#include "Enemy.h"
#include "Image.h"
#include "FrameCountTimer.h"
#include "CampaignPlayer.h"
#include "E02RoverManager.h"
#include <list>

class E01Sensor : public Enemy {
public:
	struct SpritePack {
		const Image *spriteCooldownDetail, *spriteDeath;
		Animation *spawnDetail;

		SpritePack(const Image *spriteCooldownDetail, const Image *spriteDeath, Animation *spawnDetail) {
			this->spriteCooldownDetail = spriteCooldownDetail;
			this->spriteDeath = spriteDeath;
			this->spawnDetail = spawnDetail;
		}
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_SPAWN_STATE, MODE_COOLDOWN
	}mode;

	Image *cooldownSprite;
	FrameCountTimer cooldownTimer, spawnStateTimer;
	CampaignPlayer *player;
	E02RoverManager *e02RoverManager;
	std::list<EnemyManager*> *enemyManagerList;
	std::list<InteractiveObject*> spawnStateList;
	SpritePack spritePack;
	int sonarRadius;
	BallObject *ball;

	void spawnE02Rovers();
	void drawSpawnLines();
	void spawnStateMode();
	void cooldownMode();
	void drawSensorLine();
	void drawSonarEffect();

public:
	E01Sensor(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites,
		CampaignPlayer *player, E02RoverManager *e02RoverManager, std::list<EnemyManager*> *enemyManagerList, SpritePack *spritePack);

	void update();
	void draw();

	void pause();
	void resume();
};

#endif