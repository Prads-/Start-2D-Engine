#include "SfxPool.h"

AudioCore::AudioBuffer SfxPool::sfxButtonOver1, SfxPool::sfxExplosion1, SfxPool::sfxBallShoot, SfxPool::sfxEnemyDeath,
	SfxPool::sfxPickupSpawn, SfxPool::sfxPlayerDeath, SfxPool::sfxPlayerHurt, SfxPool::sfxDetection1, SfxPool::sfxEnemyShoot1, 
	SfxPool::sfxEnemyShoot2, SfxPool::sfxSiren1, SfxPool::sfxDetection2, SfxPool::sfxBallShieldBounce;
bool SfxPool::wasInitialised = false;

void SfxPool::initialise(StartEngine *engine) {
	if (!wasInitialised) {
		engine->createAudioBuffer("Sound/Effects/ButtonOver1.ogg", &sfxButtonOver1);
		engine->createAudioBuffer("Sound/Effects/Explosion1.ogg", &sfxExplosion1);
		engine->createAudioBuffer("Sound/Effects/BallShoot.ogg", &sfxBallShoot);
		engine->createAudioBuffer("Sound/Effects/PickupSpawn.ogg", &sfxPickupSpawn);
		engine->createAudioBuffer("Sound/Effects/PlayerDeath.ogg", &sfxPlayerDeath);
		engine->createAudioBuffer("Sound/Effects/PlayerHurt.ogg", &sfxPlayerHurt);
		engine->createAudioBuffer("Sound/Effects/EnemyDeath.ogg", &sfxEnemyDeath);
		engine->createAudioBuffer("Sound/Effects/Detection1.ogg", &sfxDetection1);
		engine->createAudioBuffer("Sound/Effects/EnemyShoot1.ogg", &sfxEnemyShoot1);
		engine->createAudioBuffer("Sound/Effects/EnemyShoot2.ogg", &sfxEnemyShoot2);
		engine->createAudioBuffer("Sound/Effects/Siren1.ogg", &sfxSiren1);
		engine->createAudioBuffer("Sound/Effects/Detection2.ogg", &sfxDetection2);
		engine->createAudioBuffer("Sound/Effects/BallShieldBounce.ogg", &sfxBallShieldBounce);
		wasInitialised = true;
	}
}

void SfxPool::cleanUp(StartEngine *engine) {
	if (wasInitialised) {
		engine->deleteAudioBuffer(&sfxButtonOver1);
		engine->deleteAudioBuffer(&sfxExplosion1);
		engine->deleteAudioBuffer(&sfxBallShoot);
		engine->deleteAudioBuffer(&sfxPickupSpawn);
		engine->deleteAudioBuffer(&sfxPlayerDeath);
		engine->deleteAudioBuffer(&sfxPlayerHurt);
		engine->deleteAudioBuffer(&sfxEnemyDeath);
		engine->deleteAudioBuffer(&sfxDetection1);
		engine->deleteAudioBuffer(&sfxEnemyShoot1);
		engine->deleteAudioBuffer(&sfxEnemyShoot2);
		engine->deleteAudioBuffer(&sfxSiren1);
		engine->deleteAudioBuffer(&sfxDetection2);
		engine->deleteAudioBuffer(&sfxBallShieldBounce);
	}
}

AudioCore::AudioBuffer *SfxPool::getSfxButtonOver1() {
	return &sfxButtonOver1;
}

AudioCore::AudioBuffer *SfxPool::getSfxExplosion1() {
	return &sfxExplosion1;
}

AudioCore::AudioBuffer *SfxPool::getSfxBallShoot() {
	return &sfxBallShoot;
}

AudioCore::AudioBuffer *SfxPool::getSfxEnemyDeath() {
	return &sfxEnemyDeath;
}

AudioCore::AudioBuffer *SfxPool::getSfxPickupSpawn() {
	return &sfxPickupSpawn;
}

AudioCore::AudioBuffer *SfxPool::getSfxPlayerDeath() {
	return &sfxPlayerDeath;
}

AudioCore::AudioBuffer *SfxPool::getSfxPlayerHurt() {
	return &sfxPlayerHurt;
}

AudioCore::AudioBuffer *SfxPool::getSfxDetection1() {
	return &sfxDetection1;
}

AudioCore::AudioBuffer *SfxPool::getSfxEnemyShoot1() {
	return &sfxEnemyShoot1;
}

AudioCore::AudioBuffer *SfxPool::getSfxEnemyShoot2() {
	return &sfxEnemyShoot2;
}

AudioCore::AudioBuffer *SfxPool::getSfxSiren1() {
	return &sfxSiren1;
}

AudioCore::AudioBuffer *SfxPool::getSfxDetection2() {
	return &sfxDetection2;
}

AudioCore::AudioBuffer *SfxPool::getSfxBallShieldBounce() {
	return &sfxBallShieldBounce;
}