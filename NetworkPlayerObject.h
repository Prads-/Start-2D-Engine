#ifndef NETWORK_PLAYER_OBJECT_H
#define NETWORK_PLAYER_OBJECT_H

#include "PlayerObject.h"
#include "DataPackets.h"

class NetworkPlayerObject : public PlayerObject {
private:
	DataPacketClientInput clientInput;
	unsigned int id;
	FrameCountTimer damageAnimationTimer;
	
	void drawTargetLine() { }
	void makeHealthBarFull() { }
	void updateHealthBar() { }
	
	void updateVirtualCursor();
	void takeAwayHP(int hp = 1);
	
	bool isKeyPressed(int key);
	bool wasMouseLeftToggled();
	bool wasMouseMidToggled();
	bool wasMouseRightToggled();
	void checkCursorMovementToggle() { }

public:
	NetworkPlayerObject(StartEngine *engine, BallObject *ball, Image *font, const std::vector<BoundingBox> *bBoxes, std::list<Immobiliser> *immobilisers,
		std::string characterPath, PlayerVariables playerVariables, Image *pickupSprites, const Image *pickupCross);

	void setDataPacketClientInput(DataPacketClientInput clientInput);

	void update();
	void draw();
	void presentHUD() { }
	void renderHUD() { }
	void drawCursor() { }
	
	unsigned int getId();
	void setId(unsigned int id);

	Vector2f getVirtualCursorPosition();
};

#endif