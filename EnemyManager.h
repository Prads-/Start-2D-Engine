#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "StartEngine.h"
#include "CampaignPlayer.h"
#include "BallObject.h"
#include <vector>
#include <list>
#include "Sprite.h"

class EnemyManager {
protected:
	std::list<void*> enemies;
	std::list<void*> enemiesCollection;
	CampaignPlayer *player;
	StartEngine *engine;
	std::vector<BoundingBox> *bBoxes;
	Image shadowSprite;
	std::list<Immobiliser> *immobilisers;
	int shadowPeriodInterval;
	std::vector<Sprite*> sprites;
	int killScore;
	BallObject *ball;

	void loadSpriteListFile(std::string path, std::vector<Sprite*> *spritesOut = 0);

public:
	EnemyManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers);
	virtual ~EnemyManager();

	void setPlayer(CampaignPlayer *player);
	void setBall(BallObject *ball);
	void setShadowPeriodInterval(int shadowPeriodInterval);

	virtual void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false) = 0;
	virtual void updateAllEnemies();
	virtual void drawAllEnemies();
	virtual bool doesAnyEnemyCollideWithBB(const BoundingBox *bBox) const;
	virtual void updateGridMap();
	virtual void getEnemiesPointer(std::vector<const Enemy*> *enemiesPtrOut);

	bool isEmpty() const;
	void makeEmpty();

	virtual void createShadowSprite(uint8 alpha = 0xAF);

	virtual void pause();
	virtual void resume();

	void *getLastEnemy();
};

#endif
