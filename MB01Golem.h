#ifndef MB01GOLEM_H
#define MB01GOLEM_H

#include "PatrolRover.h"
#include "FrameCountTimer.h"
#include <list>
#include "MB01GolemShell.h"
#include "Grid.h"
#include "vector2.h"
#include "MB01Sensor.h"

#define MB01GOLEM_HEALTH			20
#define MB01GOLEM_WIDTH				70
#define MB01GOLEM_HEIGHT			70

class MB01Golem : public PatrolRover {
public:
	struct SpritePack {
		const Animation *artilleryExplosionAnimation, *mortarExplosionAnimation, *mortarFallAnimation, *artilleryShellAnimation;
		const Image *turretSprite, *shieldFullSprite, *shieldDamageSprite, *deathSprite, *sensorDeathSprite;

		SpritePack(const Animation *artilleryExplosionAnimation, const Animation *mortarExplosionAnimation, 
				const Animation *mortarFallAnimation, Animation *artilleryShellAnimation, Image *turretSprite, Image *shieldFullSprite, 
				Image *shieldDamageSprite, Image *deathSprite, Image *sensorDeathSprite) {
			this->artilleryExplosionAnimation = artilleryExplosionAnimation;
			this->mortarExplosionAnimation = mortarExplosionAnimation;
			this->mortarFallAnimation = mortarFallAnimation;
			this->artilleryShellAnimation = artilleryShellAnimation;
			this->turretSprite = turretSprite;
			this->shieldFullSprite = shieldFullSprite;
			this->shieldDamageSprite = shieldDamageSprite;
			this->deathSprite = deathSprite;
			this->sensorDeathSprite = sensorDeathSprite;
		}
	};

private:
	enum Mode {
		MODE_NORMAL, MODE_ARTILLERY_SHOOT, MODE_ARTILLERY_SEARCH, MODE_MORTAR, MODE_MORTAR_STRIKE, MODE_SCATTER
	}mode;

	enum ShieldMode {
		SHIELD_MODE_SHIELDED, SHIELD_MODE_RECHARGE, SHIELD_MODE_BROKEN
	}shieldMode;

	SpritePack spritePack;
	Animation shieldRecoveryAnimation;
	FrameCountTimer artilleryShootTimer, scatterShootTimer, mortarShootTimer, modeSelectTimer, shieldRechargeTimer, shieldRechargeDelayTimer;
	bool artilleryShootCooldown, wasImmuneInPreviousFrame, hasSeenPlayer;
	std::list<MB01GolemShell> shells;
	Vector2f lastSeenPlayerPosition;
	Grid **gridMap;
	std::list<Vector2i> pathPoints;
	std::list<MB01Sensor> sensors;
	std::vector<Sprite*> *sensorSprites;
	int turretRotation, shieldHitCounter, scatterStrikeCounter, mortarStrikeCounter;

	void modeNormal();
	void modeArtilleryShoot();
	void modeArtillerySearch();
	void modeScatter();
	void modeMortar();
	void modeMortarStrike();

	void shieldModeShielded();
	void shieldModeRecharge();
	void shieldModeBroken();
	void takeAwayHP(int hp = 1);
	void rechargeShield();

	void findPath();
	void updateRotateDegrees(Vector2f towards);
	void goToNormalMode();
	void goToScatterMode();
	void goToMortarMode(bool instantiateSensors);
	void goToMortarStrikeMode();

	void scatterShells();
	bool checkScatteredShellPosition(MB01GolemShell *shell);
	void placeShellNearPlayer();
	void checkModeSelection();
	void updateSensors();
	void updateShells();
	bool rotateTurrent(const Vector2f *direction);
	bool doPreUpdate();

public:
	MB01Golem(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
		int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, 
		Vector2f position, Grid **gridMap, std::vector<Sprite*> *sensorSprites, SpritePack *spritePack, const Animation *shieldRecoveryAnimation);

	void update();
	void draw();

	void checkCollisionWithBall();

	void pause();
	void resume();
};

#endif