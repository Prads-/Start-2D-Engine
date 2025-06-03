#include "MinionEggManager.h"
#include "MinionEgg.h"
using namespace std;

MinionEggManager::MinionEggManager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers, 
		std::list<EnemyManager*> *enemyManagers, Minion1Manager *minion1Manager, Minion2Manager *minion2Manager) 
				: EnemyManager(engine, bBoxes, immobilisers) {
	this->enemyManagers = enemyManagers;
	this->minion1Manager = minion1Manager;
	this->minion2Manager = minion2Manager;
	loadSpriteListFile("Images/enemies/MinionEgg/sprites.dat");
	holeOpenAnimation.openAnimationFile("Images/enemies/MinionEgg/holeOpen.anim");
	holeCloseAnimation.openAnimationFile("Images/enemies/MinionEgg/holeClose.anim");
	eggAnimation.openAnimationFile("Images/enemies/MinionEgg/sprite.anim");
	deathSprite.loadImage("Images/enemies/MinionEgg/Death.png");
	eggAnimation.setLoop(true);
	createShadowSprite(0x7F);
}
	
void MinionEggManager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	MinionEgg *minionEgg = new MinionEgg(engine, bBoxes, 4, ball, &shadowSprite, shadowPeriod, immobilisers, &sprites, 
		enemyManagers, minion1Manager, minion2Manager, position, &holeOpenAnimation, &holeCloseAnimation, &eggAnimation, &deathSprite);
	if (randomPosition) {
		minionEgg->setAtRandomPosition();
		minionEgg->resetPreviousPosition();
	}
	enemies.push_back(minionEgg);
	enemiesCollection.push_back(minionEgg);
}

void MinionEggManager::createShadowSprite(uint8 alpha) {
	const vector<const Image*> *eggAnimationSprites = eggAnimation.getAnimationFrames();
	const Image *baseSprite = (*eggAnimationSprites)[0];
	shadowSprite.createImage(baseSprite->getWidth(), baseSprite->getHeight());
	baseSprite->createShadowImage(shadowSprite.getModifiablePtr(), alpha);
}