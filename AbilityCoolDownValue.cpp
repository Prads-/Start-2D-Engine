#include "AbilityCoolDownValues.h"

int AbilityCoolDownValue::getCooldownValue(PickupMove move) {
	switch (move) {
		case PICKUP_GRANADE:
		case PICKUP_HOMING:
		case PICKUP_INVISIBLE_BALL:
		case PICKUP_TELEPORT_BALL:
		case PICKUP_PIERCING_BALL:
		case PICKUP_IMMOBILISER:
		case PICKUP_SHIELD:
		case PICKUP_BATTERY:
		default:
			return 600;
		case PICKUP_CALL_BALL:
			return 360;
		case PICKUP_PINBALL:
			return 960;
	}
}