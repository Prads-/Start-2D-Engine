#ifndef DUMB_BALL_OBJECT_H
#define DUMB_BALL_OBJECT_H

#include "DumbObject.h"
#include "DataPackets.h"
#include "FrameCountTimer.h"
#include "Image.h"

class DumbBallObject : public DumbObject {
private:
	DataPacketPositions::BallPositionData positionPacket;
	Image sprite, spriteIdle, spriteHit;
	float idleCircleRadius;

	void drawIdleVisibilityCircle();

public:
	DumbBallObject(StartEngine *engine);

	void setPositionPacket(DataPacketPositions::BallPositionData positionPacket);

	void draw();
};

#endif