#include "VirtualCursor.h"

#define HALF_FRAME_WIDTH		FRAME_WIDTH / 2.0f
#define HALF_FRAME_HEIGHT		FRAME_HEIGHT / 2.0f

VirtualCursor::VirtualCursor(StartEngine *engine) {
	spriteCursor.loadImage("Images/cursor.png");
	width = spriteCursor.getWidth();
	height = spriteCursor.getHeight();
	this->engine = engine;
	position = Vector2f(HALF_FRAME_WIDTH, HALF_FRAME_HEIGHT);
	engine->getScreenResolution(halfScreenResolutionX, halfScreenResolutionY);
	halfScreenResolutionX /= 2;
	halfScreenResolutionY /= 2;
}

void VirtualCursor::update(float mouseX, float mouseY, bool putMouseInMiddle) {
	Vector2f direction = Vector2f(mouseX, mouseY) - Vector2f(HALF_FRAME_WIDTH, HALF_FRAME_HEIGHT);
	position.x += direction.x;
	position.y += direction.y;
	if (position.x < 0.0f) {
		position.x = 0.0f;
	} else if (position.x >= FRAME_WIDTH) {
		position.x = FRAME_WIDTH;
	}
	if (position.y < 0.0f) {
		position.y = 0.0f;
	} else if (position.y >= FRAME_HEIGHT) {
		position.y = FRAME_HEIGHT;
	}
	if (putMouseInMiddle) {
		engine->setMouseCursorInScreenSpace(halfScreenResolutionX, halfScreenResolutionY);
	}
}

void VirtualCursor::draw() {
	engine->draw(&spriteCursor, 0, false, false, false, position.x - width / 2, position.y - height / 2);
}

Vector2f VirtualCursor::getPosition() {
	return position;
}

void VirtualCursor::setPosition(Vector2f position) {
	this->position = position;
}