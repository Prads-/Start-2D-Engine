#ifndef SCREEN_H
#define SCREEN_H

#include "StartEngine.h"
#include "BoundingBox.h"
#include "VariableSizeFont.h"

class Screen {
public:
	enum MoveOn {
		MOVE_ON_NONE, MOVE_ON_NEXT, MOVE_ON_PREVIOUS
	};

protected:
	StartEngine *engine;
	MoveOn moveOn;
	static Image background, edgeButtonBackgroundDefault, edgeButtonBackgroundHighlight, 
		middleButtonBackgroundDefault, middleButtonBackgroundHighlight, middleButtonBackgroundDefault2,
		lockedSprite;
	static VariableSizeFont customFont;

	void nextScreen();
	void previousScreen();

public:

	Screen(StartEngine *engine);
	virtual ~Screen() { }

	MoveOn getMoveOn();
	virtual void resetScreen();

	virtual void update() = 0;
	virtual void renderScreen() = 0;
	virtual void *getUserInput();

	static const Image *getBackground();
	static const VariableSizeFont *getCustomFont();
	static const Image *getEdgeButtonBackgroundDefault();
	static const Image *getEdgeButtonBackgroundHighlight();
	static const Image *getMiddleButtonBackgroundDefault();
	static const Image *getMiddleButtonBackgroundHighlight();
	static const Image *getMiddleButtonBackgroundDefault2();
};

#endif