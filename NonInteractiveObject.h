#ifndef NON_INTERACTIVE_OBJECT_H
#define NON_INTERACTIVE_OBJECT_H

#include "declarations.h"
#include <vector>
#include "StartEngine.h"
#include "GameObject.h"
#include "vector2.h"

class NonInteractiveObject : public GameObject {
protected:
	std::vector<Vector2i> positions;

public:
	NonInteractiveObject(StartEngine *engine);

	void pushBackPositions(int32 *positions, int size);
	void pushBackPosition(Vector2i position);

	const std::vector<Vector2i> *getPositions();
};

#endif