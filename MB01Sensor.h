#ifndef MB01SENSOR_H
#define MB01SENSOR_H

#include "PatrolRover.h"

class MB01Sensor : public PatrolRover {
private:
	bool hasSeenPlayer;
	const Image *deathSprite;
	
	void drawStunEffect();

public:
	MB01Sensor(StartEngine *engine, const std::vector<BoundingBox> *bBoxes, int health, BallObject *ball, Image *shadowSprite, 
			int initialShadowPeriodInterval, std::list<Immobiliser> *immobilisers, std::vector<Sprite*> *sprites, CampaignPlayer *player, 
			Vector2f position, const Image *deathSprite);

	bool getHasSeenPlayer() const;

	void update();
	void draw();
};

#endif