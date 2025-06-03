#ifndef ABILITY_COOLDOWN_VALUES_H
#define ABILITY_COOLDOWN_VALUES_H

#include "build.h"
#include "declarations.h"
#include "PickupEnums.h"

class AbilityCoolDownValue {
public:
	int getCooldownValue(PickupMove move);
};

#endif