#include "Minion1Manager.h"
#include "EnemyScore.h"
using namespace std;

Minion1Manager::Minion1Manager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers)
				: EnemyManager(engine, bBoxes, immobilisers) {
	spriteAttack.loadImage("Images/enemies/minion1/AttackStateEffect.png");
	spriteDeath.loadImage("Images/enemies/minion1/DeathSprite.png");
	loadSpriteListFile("Images/enemies/minion1/sprites.dat");
	createShadowSprite();
	speed = 2.7f;
	killScore = ENEMY_SCORE_MINION_1;
}

void Minion1Manager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	Minion1::SpritePack spritePack = Minion1::SpritePack(&spriteAttack, &spriteDeath);
	Minion1 *minion1 = new Minion1(engine, bBoxes, 1, ball, &shadowSprite, shadowPeriod, immobilisers, speed, player, position, &sprites, &spritePack);
	if (randomPosition) {
		minion1->setAtRandomPosition();
	}
	enemies.push_back(minion1);
	enemiesCollection.push_back(minion1);
}