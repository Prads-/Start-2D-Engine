#ifndef SPINNER_H
#define SPINNER_H

#include "GuiComponent.h"
#include "Button.h"
#include "Image.h"
#include <functional>

class Spinner : public GuiComponent {
private:
	Button btnUp, btnDown;
	static Image btnSprite;
	int minLimit, maxLimit;
	int value;
	uint32 borderColor, textColor;
	const Image *font;
	std::function<void()> *valueChangeCallBack;

	void upClicked();
	void downClicked();

public:
	Spinner(StartEngine *engine, Vector2i position, int width, bool isAlphaBlended, int minLimit, int maxLimit, uint32 borderColor, uint32 textColor, 
		const Image *font);

	int getValue();
	void setValue(int value);
	void setValueChangedCallback(std::function<void()> *valueChangeCallBack);

	void update(bool wasMouseLeftToggled);
	void draw();
};

#endif