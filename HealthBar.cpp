#include "HealthBar.h"

#define HEALTH_BAR_WIDTH		12
#define HEALTH_BAR_HEIGHT		102
#define HEALTH_BAR_FRAME_COLOR	0xFF7F7F7F
#define HEALTH_BAR_COLOR		0xFF00FF00

HealthBar::HealthBar() {
	healthBar.createImage(HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);
}

void HealthBar::makeHealthBarFull() {
	uint32 *healthBarImage = healthBar.getModifiablePtr();
	for (int i = 0; i < HEALTH_BAR_WIDTH; ++i) {
		healthBarImage[i] = HEALTH_BAR_FRAME_COLOR;
	}
	for (int i = 12 * (HEALTH_BAR_HEIGHT - 1); i < 12 * HEALTH_BAR_HEIGHT; ++i) {
		healthBarImage[i] = HEALTH_BAR_FRAME_COLOR;
	}
	for (int y = 1; y < (HEALTH_BAR_HEIGHT - 1); ++y) {
		healthBarImage[y * HEALTH_BAR_WIDTH] = HEALTH_BAR_FRAME_COLOR;
		healthBarImage[(HEALTH_BAR_WIDTH - 1) + y * HEALTH_BAR_WIDTH] = HEALTH_BAR_FRAME_COLOR;
		for (int x = 1; x < HEALTH_BAR_WIDTH - 1; ++x) {
			healthBarImage[x + y * HEALTH_BAR_WIDTH] = HEALTH_BAR_COLOR;
		}
	}
}

void HealthBar::updateHealthBar(int health, int maxHealth) {
	if (health < 0) {
		return;
	}
	int healthPercentage = (100 / maxHealth) * health;
	int barSize = 100 - healthPercentage;
	uint32 *healthBarImage = healthBar.getModifiablePtr();
	healthBarImage += HEALTH_BAR_WIDTH;
	barSize *= HEALTH_BAR_WIDTH;
	for (int i = 0; i < barSize; ++i) {
		healthBarImage[i] = HEALTH_BAR_FRAME_COLOR;
	}
}

const Image *HealthBar::getHealthBar() {
	return &healthBar;
}