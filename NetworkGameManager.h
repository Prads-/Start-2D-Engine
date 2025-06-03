#ifndef NETWORK_GAME_MANAGER_H
#define NETWORK_GAME_MANAGER_H

#include "StartEngine.h"

class NetworkGameManager {
protected:
	StartEngine *engine;
	Image *font, *pickupSprites, *pickupCross;
	std::vector<PrioritisedAsset> prioritisedAssets;
	std::vector<Image*> prioritisedImageCollector;
	uint32 positionPacketId;
	Image *shootFrame;

	void packetRecieved(uint8 *packet);
	virtual void checkPacket(DataPacketDataType *packet) = 0;
	virtual void cleanUpGame();

public:
	NetworkGameManager(StartEngine *engine, Image *font, Image *pickupSprites, Image *shootFrame, Image *pickupCross);
	virtual ~NetworkGameManager() { }
};

#endif