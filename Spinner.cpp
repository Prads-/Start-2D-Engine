#include "Spinner.h"
#include "stringExtension.h"
using namespace StringExtension;

Image Spinner::btnSprite("Images/spinnerButton.png");

Spinner::Spinner(StartEngine *engine, Vector2i position, int width, bool isAlphaBlended, int minLimit, int maxLimit, uint32 borderColor, 
				 uint32 textColor, const Image *font)
		: GuiComponent(engine, position, width, 32, false), 
		btnDown(engine, Vector2i(position.x + width + 1, position.y + 17), false, [this]() { downClicked(); }),
		btnUp(engine, Vector2i(position.x + width + 1, position.y), false, [this]() { upClicked(); }) {
	this->minLimit = minLimit;
	this->maxLimit = maxLimit;
	this->borderColor = borderColor;
	this->textColor = textColor;
	this->font = font;
	value = minLimit;
	btnUp.setButtonSprite(&btnSprite);
	btnDown.setButtonSprite(&btnSprite);
	btnDown.setFlipY(true);
	valueChangeCallBack = 0;
}

void Spinner::upClicked() {
	if (value < maxLimit) {
		++value;
		if (valueChangeCallBack) {
			(*valueChangeCallBack)();
		}
	}
}

void Spinner::downClicked() {
	if (value > minLimit) {
		--value;
		if (valueChangeCallBack) {
			(*valueChangeCallBack)();
		}
	}
}

void Spinner::update(bool wasMouseLeftToggled) {
	btnUp.update(wasMouseLeftToggled);
	btnDown.update(wasMouseLeftToggled);
}

void Spinner::draw() {
	Vector2i pos = position;
	engine->drawLine(pos, Vector2f(1.0f, 0.0f), width, borderColor);
	engine->drawLine(pos, Vector2f(0.0f, 1.0f), 32, borderColor);
	pos.y += 32.0f;
	engine->drawLine(pos, Vector2f(1.0f, 0.0f), width, borderColor);
	pos.x += width;
	engine->drawLine(pos, Vector2f(0.0f, -1.0f), 32, borderColor);
	btnUp.draw();
	btnDown.draw();
	engine->drawText(intToStr(value), font, 32, 32, 16, textColor, position.x + 5, position.y + 5); 
}

int Spinner::getValue() {
	return value;
}

void Spinner::setValue(int value) {
	if (value >= minLimit && value <= maxLimit) {
		this->value = value;
	}
}

void Spinner::setValueChangedCallback(std::function<void()> *valueChangeCallBack) {
	this->valueChangeCallBack = valueChangeCallBack;
}