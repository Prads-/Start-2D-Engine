#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "InteractiveObject.h"
#include "StartEngine.h"
#include "Animation.h"

class Competitor;

class Explosion : public InteractiveObject {
private:
	static Explosion *explosion;
	bool isActive;
	Animation animation;
	FrameCountTimer explosionTimer;
	Competitor *shooter;
	float radius;

	Explosion(StartEngine *engine);

public:
	static Explosion *getInstance(StartEngine *engine);
	static Explosion *getInstance();
	static void cleanupInstance();

	void setPosition(Vector2f position);

	float getRadius();
	bool getIsActive();
	void setIsActive(bool isActive);
	Competitor *getShooter();
	void setShooter(Competitor *shooter);

	void update();
	void draw();

	void pause();
	void resume();
};

#endif