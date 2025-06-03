#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "declarations.h"
#include "StartEngine.h"
#include "vector2.h"

class GameObject {
protected:
	StartEngine *engine;

	GameObject(StartEngine *engine);
public:
	virtual ~GameObject() { }
	virtual void draw() = 0;
};

#endif