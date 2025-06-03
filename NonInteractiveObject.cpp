#include "NonInteractiveObject.h"
using namespace std;

NonInteractiveObject::NonInteractiveObject(StartEngine *engine) : GameObject(engine) { }

void NonInteractiveObject::pushBackPositions(int32 *positions, int size) {
	int numberOfPosition = size / 2;
	int counter = 0;

	while (numberOfPosition-- != 0) {
		this->positions.push_back(Vector2i(positions[counter], positions[counter + 1]));
		counter += 2;
	}
}

void NonInteractiveObject::pushBackPosition(Vector2i position) {
	positions.push_back(position);
}

const vector<Vector2i> *NonInteractiveObject::getPositions() {
	return &positions;
}