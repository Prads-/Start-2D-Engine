#include "NetworkGameManager.h"

NetworkGameManager::NetworkGameManager(StartEngine *engine, Image *font, Image *pickupSprites, Image *shootFrame, Image *pickupCross) {
	this->engine = engine;
	this->font = font;
	this->pickupSprites = pickupSprites;
	this->shootFrame = shootFrame;
	this->pickupCross = pickupCross;
}

void NetworkGameManager::packetRecieved(uint8 *packet) {
	checkPacket((DataPacketDataType*)packet);
}

void NetworkGameManager::cleanUpGame() {
	for (Image *image : prioritisedImageCollector) {
		delete image;
	}
	prioritisedAssets.clear();
	prioritisedImageCollector.clear();
}