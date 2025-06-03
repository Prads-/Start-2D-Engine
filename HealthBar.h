#ifndef HEALTH_BAR_H
#define HEALTH_BAR_H

#include "Image.h"

class HealthBar {
private:
	Image healthBar;

public:
	HealthBar();

	void makeHealthBarFull();
	void updateHealthBar(int health, int maxHealth);

	const Image *getHealthBar();
};

#endif