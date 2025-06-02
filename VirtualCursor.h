#ifndef VIRTUAL_CURSOR_H
#define VIRTUAL_CURSOR_H

#include "Image.h"
#include "vector2.h"
#include "StartEngine.h"

class VirtualCursor {
private:
	Image spriteCursor;
	Vector2f position;
	StartEngine *engine;
	int width, height;
	int halfScreenResolutionX, halfScreenResolutionY;

public:
	VirtualCursor(StartEngine *engine);
	
	void update(float mouseX, float mouseY, bool putMouseInMiddle = true);
	void draw();
	
	Vector2f getPosition();
	void setPosition(Vector2f position);
};

#endif