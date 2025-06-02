#ifndef GUI_COMPONENT_BUTTON_H
#define GUI_COMPONENT_BUTTON_H

#include "GuiComponent.h"
#include <functional>
#include <string>
#include "VariableSizeFont.h"

class Button : public GuiComponent {
private:
	const Image *buttonSprite, *onMouseOverSprite;
	Image *disableSprite;
	std::function<void()> clickedEvent;
	bool wasButtonSpriteLoaded, wasOnMouseOverSpriteLoaded;
	bool isDisabled, flipImageX, flipImageY, sfxMouseOverWasPlayed, showBorder;
	std::string caption;
	int captionOffsetX, captionOffsetY;
	const Image *captionFont;
	const VariableSizeFont *captionVariableSizeFont;
	uint32 captionFontColorDefault, captionFontColorHighlight, captionFontColorDisabled, borderColor;

	void updateSize();
	void updateDisableSprite();

public:
	Button(StartEngine *engine, Vector2i position, bool isAlphaBlended, std::function<void()> clickedEvent);
	~Button();

	void setButtonSprite(const Image *buttonSprite);
	void setMouseOverSprite(const Image *onMouseOverSprite);
	void setIsDisabled(bool isDisabled, bool updateSprite = false);
	void setIsDisabled_NoSprite(bool isDisabled);
	void setClickedEvent(std::function<void()> clickedEvent);

	void loadButtonSprite(std::string spritePath);
	void loadOnMouseOverSprite(std::string spritePath);

	void update(bool wasMouseLeftToggled);
	void draw();

	void setFlipX(bool flipX);
	void setFlipY(bool flipY);

	void setCaption(std::string caption, int captionOffsetX, int captionOffsetY, const Image *font, const VariableSizeFont *variableSizeFont,
		uint32 captionFontColorDefault, uint32 captionFontColorHighlight, uint32 captionFontColorDisabled = 0);

	void setShowBorder(bool show);
	void setBorderColor(uint32 color);
};

#endif