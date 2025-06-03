#ifndef MB01GOLEM_SHELL_H
#define MB01GOLEM_SHELL_H

#include "Animation.h"
#include "FrameCountTimer.h"
#include "CampaignPlayer.h"
#include "vector2.h"
#include "InteractiveObject.h"
#include <vector>

class MB01GolemShell : public InteractiveObject {
private:
	enum Mode {
		MODE_NORMAL, MODE_EXPLOSION, MODE_INACTIVE
	}mode;

	Animation shellExplosionAnimation, shellAnimation;
	CampaignPlayer *player;
	FrameCountTimer explosionTimer, explodeTimer;
	int explosionRadius;
	bool explodeWhenCollisionWithPlayer;

	void normalMode();
	void explosionMode();
	
public:
	MB01GolemShell(StartEngine *engine, CampaignPlayer *player, 
		int explosionInterval, int explodeInterval, float speed,
		Vector2f position, Vector2f lookAt, const std::vector<BoundingBox> *bBoxes, int explosionRadius, bool explodeWhenCollisionWithPlayer,
		const Animation *shellExplosionAnimation, const Animation *shellAnimation);

	void update();
	void draw();

	bool isActive();
	int getExplosionRadius();
};

#endif