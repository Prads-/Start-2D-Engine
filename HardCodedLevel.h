//#ifndef HARD_CODED_LEVEL_H
//#define HARD_CODED_LEVEL_H
//
//#include "declarations.h"
//#include "StartEngine.h"
//#include "PlayerObject.h"
//#include "AIObject.h"
//#include "FileLoader.h"
//#include <string>
//#include <vector>
//#include "NetworkGameManager.h"
//#include "PickupItem.h"
//#include "Explosion.h"
//#include "Immobiliser.h"
//#include <list>
//
//class HardCodedLevel {
//private:
//	StartEngine *engine;
//	PlayerObject *player;
//	BallObject *ball;
//	AIObject *aiObject, *aiObject2;
//	Image font, engineLogo, marioPicture;
//	std::string result;
//	Timer logoTimer;
//	std::vector<BoundingBox> bBoxes;
//	Timer fpsTimer;
//	NetworkGameManager *networkGameManager;
//	PickupItem *pickupItem;
//	Image pickupGranade, pickupHoming, pickupImmobiliser, pickupPinball, pickupShield, pickupPiercingBall, pickupBattery, 
//		pickupInvisibleBall, pickupTeleportBall;
//	Explosion *explosion;
//	std::list<Immobiliser> immobilisers;
//
//	void createBackground(const std::vector<FileLoader::LevelAsset> *levelAssets);
//	void renderLogo();
//	void renderTitle();
//	void renderLocalGame();
//	void renderNetworkGame();
//	void renderResult();
//	void renderNetworkGameStart();
//	void startLocalGame();
//	void startNetworkGame();
//
//public:
//	HardCodedLevel(StartEngine *engine);
//	~HardCodedLevel();
//
//	void renderLevel();
//};
//
//#endif