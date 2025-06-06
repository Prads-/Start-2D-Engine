#ifndef PICKUP_ENUMS_H
#define PICKUP_ENUMS_H

enum PickupMove {
	PICKUP_GRANADE = 0, PICKUP_HOMING = 1, PICKUP_PINBALL = 2, PICKUP_PIERCING_BALL = 3, PICKUP_INVISIBLE_BALL = 4, PICKUP_CALL_BALL = 5,
	PICKUP_IMMOBILISER = 6, PICKUP_SHIELD = 7, PICKUP_BATTERY = 8, PICKUP_TELEPORT_BALL = 9,
	PICKUP_NONE = 10,
	PICKUP_MAX_VALUE = 10
};

enum BallType {
	BALL_TYPE_NORMAL = PICKUP_NONE, 
	BALL_TYPE_GRANADE = PICKUP_GRANADE, BALL_TYPE_HOMING = PICKUP_HOMING, BALL_TYPE_PINBALL = PICKUP_PINBALL, 
	BALL_TYPE_PIERCING = PICKUP_PIERCING_BALL, BALL_TYPE_INVISIBLE = PICKUP_INVISIBLE_BALL, BALL_TYPE_CALL_BALL = PICKUP_CALL_BALL
};


#endif