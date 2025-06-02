#ifndef CHECK_BOX_H
#define CHECK_BOX_H

#include "GuiComponent.h"
#include "Image.h"

class CheckBox : public GuiComponent {
private:
	static Image checked, unchecked;
	bool isChecked;
public:
	CheckBox(StartEngine *engine, Vector2i position, bool isAlphaBlended);

	bool getIsChecked();

	void update(bool wasMouseLeftToggled);
	void draw();
};

#endif