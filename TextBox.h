#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "GuiComponent.h"
#include <string>
#include "FrameCountTimer.h"
#include "BoundingBox.h"
#include "Image.h"

class TextBox : public GuiComponent {
private:
	int maxLength, cursorPosition, charOffset, maxShownCharacters;
	std::string text, shownText;
	bool isFocused, discardKeyPressTimer, showCursor;
	FrameCountTimer keyPressTimer, cursorBlinkTimer;
	BoundingBox bBox;
	uint32 borderColor, focusedBorderColor, textColor, cursorColor;
	Image *font;

	void checkKeyPressed();
	void charKeyPressed(char character);
	void arrowKeyPressed(char val);
	void deleteKeyPressed();
	void backspaceKeyPressed();

	void updateCharOffsetAndShownText();

public:
	TextBox(StartEngine *engine, Vector2i position, int width, int maxLength, uint32 borderColor, uint32 focusedBorderColor, Image *font, uint32 textColor, uint32 cursorColor);

	void update(bool wasMouseLeftToggled);
	void draw();

	std::string getText();
	void setText(std::string text);
};

#endif