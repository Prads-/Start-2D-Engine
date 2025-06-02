#ifndef DUMB_OBJECT_H
#define DUMB_OBJECT_H

#include "StartEngine.h"
#include "vector2.h"
#include "DataPackets.h"

class DumbObject {
protected:
	StartEngine *engine;

public:
	DumbObject(StartEngine *engine);

	virtual void draw() = 0;
};

#endif