#ifndef SFX_POOL_H
#define SFX_POOL_H

#include "StartEngine.h"

class SfxPool {
private:
	static AudioCore::AudioBuffer sfxButtonOver1, sfxExplosion1, sfxBallShoot, sfxEnemyDeath, sfxPickupSpawn,
		sfxPlayerDeath, sfxPlayerHurt, sfxDetection1, sfxEnemyShoot1, sfxEnemyShoot2, sfxSiren1, sfxDetection2, sfxBallShieldBounce;
	static bool wasInitialised;

public:
	static void initialise(StartEngine *engine);
	static void cleanUp(StartEngine *engine);

	static AudioCore::AudioBuffer *getSfxButtonOver1();
	static AudioCore::AudioBuffer *getSfxExplosion1();
	static AudioCore::AudioBuffer *getSfxBallShoot();
	static AudioCore::AudioBuffer *getSfxEnemyDeath();
	static AudioCore::AudioBuffer *getSfxPickupSpawn();
	static AudioCore::AudioBuffer *getSfxPlayerDeath();
	static AudioCore::AudioBuffer *getSfxPlayerHurt();
	static AudioCore::AudioBuffer *getSfxDetection1();
	static AudioCore::AudioBuffer *getSfxEnemyShoot1();
	static AudioCore::AudioBuffer *getSfxEnemyShoot2();
	static AudioCore::AudioBuffer *getSfxSiren1();
	static AudioCore::AudioBuffer *getSfxDetection2();
	static AudioCore::AudioBuffer *getSfxBallShieldBounce();
};

#endif