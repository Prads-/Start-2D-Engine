#include "CheckBox.h"

Image CheckBox::checked("Images/chkBoxChecked.png");
Image CheckBox::unchecked("Images/chkBoxUnchecked.png");

CheckBox::CheckBox(StartEngine *engine, Vector2i position, bool isAlphaBlended)
		: GuiComponent(engine, position, checked.getWidth(), checked.getHeight(), isAlphaBlended) {
	isChecked = false;
}

bool CheckBox::getIsChecked() {
	return isChecked;
}

void CheckBox::update(bool wasMouseLeftToggled) {
	if (wasMouseLeftToggled && engine->isCursorInsideBB(&bBox)) {
		isChecked ^= true;
	}
}

void CheckBox::draw() {
	Image *sprite;

	if (isChecked) {
		sprite = &checked;
	} else {
		sprite = &unchecked;
	}

	engine->draw(sprite, 0, false, false, false, position.x, position.y);
}