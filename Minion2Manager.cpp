#include "Minion2Manager.h"
#include "EnemyScore.h"
using namespace std;

Minion2Manager::Minion2Manager(StartEngine *engine, std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers) 
				: EnemyManager(engine, bBoxes, immobilisers) {
	spriteAttackEffect.loadImage("Images/enemies/minion2/AttackEffect.png");
	spriteAttackState.loadImage("Images/enemies/minion2/AttackState.png");
	spriteChasingState.loadImage("Images/enemies/minion2/ChaseState.png");
	spriteConfusedEffect.loadImage("Images/enemies/minion2/LastKnownLocationSearchEffect.png");
	spriteConfusedState.loadImage("Images/enemies/minion2/SearchState.png");
	spriteDeath.loadImage("Images/enemies/minion2/DeathSprite.png");
	spriteDefaultState.loadImage("Images/enemies/minion2/DefaultState.png");
	spriteHitStunnedState.loadImage("Images/enemies/minion2/ImmobileState.png");
	spriteSightDetectionEffect.loadImage("Images/enemies/minion2/SightDetectionEffect.png");
	loadSpriteListFile("Images/enemies/minion2/sprites.dat");
	createShadowSprite();
	speed = 2.0f;
	chasingSpeed = 4.0f;
	killScore = ENEMY_SCORE_MINION_2;
}

void Minion2Manager::addAnEnemy(Vector2f position, bool isInInitialShadowPeriod, bool randomPosition) {
	int shadowPeriod = (isInInitialShadowPeriod) ? shadowPeriodInterval : 0;
	Minion2::SpritePack spritePack(&spriteDefaultState, &spriteAttackState, &spriteChasingState, &spriteConfusedState, &spriteHitStunnedState,
		&spriteConfusedEffect, &spriteSightDetectionEffect, &spriteDeath, &spriteAttackEffect);
	Minion2 *minion2 = new Minion2(engine, bBoxes, 2, ball, &shadowSprite, shadowPeriod, immobilisers, speed, chasingSpeed, player, position, &sprites, &spritePack);
	if (randomPosition) {
		minion2->setAtRandomPosition();
	}
	enemies.push_back(minion2);
	enemiesCollection.push_back(minion2);
}