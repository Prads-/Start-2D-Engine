#ifndef E01SENSOR_MANAGER_H
#define E01SENSOR_MANAGER_H

#include "EnemyManager.h"
#include "E02RoverManager.h"
#include <list>


class E01SensorManager : public EnemyManager {
private:
	std::list<EnemyManager*> *enemyManagers;
	E02RoverManager *e02RoverManager;
	Image spriteCooldownDetail, spriteDeath;
	Animation spawningDetail;

public:
	E01SensorManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers, 
		E02RoverManager *e02RoverManager, std::list<EnemyManager*> *enemyManagers);

	void addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition = false);
};

#endif