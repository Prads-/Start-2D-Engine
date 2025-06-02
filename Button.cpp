#include "Button.h"

Button::Button(StartEngine *engine, Vector2i position, bool isAlphaBlended, std::function<void()> clickedEvent)
		: GuiComponent(engine, position, 0, 0, isAlphaBlended) {
	this->clickedEvent = clickedEvent;
	wasButtonSpriteLoaded = false;
	wasOnMouseOverSpriteLoaded = false;
	isDisabled = false;
	buttonSprite = 0;
	onMouseOverSprite = 0;
	disableSprite = 0;
	flipImageX = false;
	flipImageY = false;
	sfxMouseOverWasPlayed = false;
	captionFont = 0;
	captionVariableSizeFont = 0;
	showBorder = false;
	borderColor = 1;
}

Button::~Button() {
	if (wasButtonSpriteLoaded) {
		delete buttonSprite;
	}
	if (wasOnMouseOverSpriteLoaded) {
		delete onMouseOverSprite;
	}
	if (disableSprite) {
		delete disableSprite;
	}
}

void Button::setButtonSprite(const Image *buttonSprite) {
	if (wasButtonSpriteLoaded) {
		delete buttonSprite;
	}
	this->buttonSprite = buttonSprite;
	updateSize();
}

void Button::setMouseOverSprite(const Image *onMouseOverSprite) {
	this->onMouseOverSprite = onMouseOverSprite;
}

void Button::setIsDisabled(bool isDisabled, bool updateSprite) {
	this->isDisabled = isDisabled;
	if (disableSprite) {
		if (updateSprite) {
			delete disableSprite;
			updateDisableSprite();
		}
	} else {
		updateDisableSprite();
	}
}

void Button::setIsDisabled_NoSprite(bool isDisabled) {
	this->isDisabled = isDisabled;
}

void Button::setClickedEvent(std::function<void()> clickedEvent) {
	this->clickedEvent = clickedEvent;
}

void Button::updateDisableSprite() {
	disableSprite = new Image(*buttonSprite);
	engine->makeImageMonochrome(disableSprite);
}

void Button::loadButtonSprite(std::string spritePath) {
	if (wasButtonSpriteLoaded) {
		delete buttonSprite;
	}
	buttonSprite = new Image(spritePath);
	wasButtonSpriteLoaded = true;
	updateSize();
}

void Button::update(bool wasMouseLeftToggled) {
	if (!isDisabled && engine->isCursorInsideBB(&bBox)) {
		if (wasMouseLeftToggled) {
			clickedEvent();
		}
	}

}

void Button::draw() {
	bool hover = engine->isCursorInsideBB(&bBox);
	const Image *sprite;
	if (disableSprite && isDisabled) {
		sprite = disableSprite;
	} else if (onMouseOverSprite) {
		if (hover) {
			sprite = onMouseOverSprite;
		} else {
			sprite = buttonSprite;
		}
	} else {
		sprite = buttonSprite;
	}
	if (sprite) engine->draw(sprite, 0, flipImageX, flipImageY, isAlphaBlended, position.x, position.y);
	
	if (caption.size() > 0) {
		uint32 textColor;
		if (isDisabled) {
			textColor = captionFontColorDisabled;
		} else if (hover) {
			textColor = captionFontColorHighlight;
		} else {
			textColor = captionFontColorDefault;
		}
		if (captionFont) {
			engine->drawText(caption, captionFont, 32, 32, 16, textColor, position.x + captionOffsetX, position.y + captionOffsetY);
		} else if (captionVariableSizeFont) {
			engine->drawText(caption, captionVariableSizeFont, 16, 5, textColor, position.x + captionOffsetX, position.y + captionOffsetY);
		}
	}

	if (showBorder) {
		engine->drawLine(bBox.minP, Vector2f(1.0f, 0.0f), width, borderColor);
		engine->drawLine(bBox.minP, Vector2f(0.0f, 1.0f), height, borderColor);
		engine->drawLine(Vector2f(bBox.maxP.x, bBox.minP.y), Vector2f(0.0f, 1.0f), height, borderColor);
		engine->drawLine(Vector2f(bBox.minP.x, bBox.maxP.y), Vector2f(1.0f, 0.0f), width, borderColor);
	}
}

void Button::loadOnMouseOverSprite(std::string spritePath) {
	onMouseOverSprite = new Image(spritePath);
	wasOnMouseOverSpriteLoaded = true;
}

void Button::updateSize() {
	width = buttonSprite->getWidth();
	height = buttonSprite->getHeight();
	calculateBB();
}

void Button::setFlipX(bool flipX) {
	this->flipImageX = flipX;
}

void Button::setFlipY(bool flipY) {
	this->flipImageY = flipY;
}

void Button::setCaption(std::string caption, int captionOffsetX, int captionOffsetY, const Image *font, const VariableSizeFont *variableSizeFont,
						uint32 captionFontColorDefault, uint32 captionFontColorHighlight, uint32 captionFontColorDisabled) {
	this->caption = caption;
	this->captionOffsetX = captionOffsetX;
	this->captionOffsetY = captionOffsetY;
	this->captionFont = font;
	this->captionVariableSizeFont = variableSizeFont;
	this->captionFontColorDefault = captionFontColorDefault;
	this->captionFontColorHighlight = captionFontColorHighlight;
	this->captionFontColorDisabled = captionFontColorDisabled;

	if (font) {
		setSize(captionOffsetX * 2 + 16 * caption.size(), captionOffsetY * 2 + 20);
	}
}

void Button::setShowBorder(bool show) {
	showBorder = show;
}

void Button::setBorderColor(uint32 color) {
	borderColor = color;
}