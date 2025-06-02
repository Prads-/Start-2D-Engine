#ifndef IMMOBILISER_H
#define IMMOBILISER_H

#include "InteractiveObject.h"
#include "Animation.h"
#include "StartEngine.h"
#include <string>

class Competitor;

class Immobiliser : public InteractiveObject {
private:
	static Animation animation;
	bool isActive;
	int frameWidth, frameHeight;
	Competitor *thrower;

public:
	Immobiliser(StartEngine *engine);
	static void addAnimationFrame(std::string framePath);

	bool getIsActive();
	const Competitor *getThrower() const;

	void setThrower(Competitor *thrower);

	void update();
	void draw();
};

#endif