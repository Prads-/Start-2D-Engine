#ifndef GUI_COMPONENT_H
#define GUI_COMPONENT_H

#include "StartEngine.h"
#include "vector2.h"
#include "BoundingBox.h"

class GuiComponent {
protected:
	int width, height;
	Vector2i position;
	BoundingBox bBox;
	StartEngine *engine;
	bool isAlphaBlended;

	void calculateBB();

public:
	GuiComponent(StartEngine *engine, Vector2i position, int width, int height, bool isAlphaBlended);
	virtual ~GuiComponent() { }

	void setSize(int width, int height);
	void setPosition(Vector2i position);
	void setIsAlphaBlended(bool isAlphaBlended);

	Vector2i getPosition();

	virtual void update(bool wasMouseLeftToggled) = 0;
	virtual void draw() = 0;
};

#endif