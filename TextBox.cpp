#include "TextBox.h"

TextBox::TextBox(StartEngine *engine, Vector2i position, int width, int maxLength, uint32 borderColor, uint32 focusedBorderColor, Image *font, 
				 uint32 textColor, uint32 cursorColor)
		: GuiComponent(engine, position, width, 32, false), keyPressTimer(10), cursorBlinkTimer(30) {
	this->maxLength = maxLength;
	bBox.minP = position;
	bBox.maxP = Vector2f(position.x + width, position.y + 32);
	isFocused = false;
	keyPressTimer.startTimer();
	discardKeyPressTimer = true;
	charOffset = 0;
	cursorPosition = 0;
	maxShownCharacters = width / 16;
	this->borderColor = borderColor;
	this->focusedBorderColor = focusedBorderColor;
	this->textColor = textColor;
	this->font = font;
	this->cursorColor = cursorColor;
	showCursor = true;
	cursorBlinkTimer.startTimer();
}

void TextBox::update(bool wasMouseLeftToggled) {
	if (isFocused) {
		if (wasMouseLeftToggled && !engine->isCursorInsideBB(&bBox)) {
			isFocused = false;
		} else {
			if (discardKeyPressTimer || keyPressTimer.checkTimer()) {
				checkKeyPressed();
			}
			if (cursorBlinkTimer.checkTimer()) {
				showCursor ^= true;
			}
		}
	} else {
		if (wasMouseLeftToggled && engine->isCursorInsideBB(&bBox)) {
			isFocused = true;
			showCursor = true;
			cursorBlinkTimer.startTimer();
		}
	}
}

void TextBox::draw() {
	Vector2i pos;
	uint32 bColor;
	if (isFocused) {
		bColor = focusedBorderColor;

		if (showCursor) {
			pos.x = position.x + 5 + (cursorPosition - charOffset) * 16;
			pos.y = position.y + 5;
			engine->drawLine(pos, Vector2f(0.0f, 1.0f), 22, cursorColor);
		}
	} else {
		bColor = borderColor;
	}
	pos = position;
	engine->drawLine(pos, Vector2f(1.0f, 0.0f), width, bColor);
	engine->drawLine(pos, Vector2f(0.0f, 1.0f), 32, bColor);
	pos.y += 32.0f;
	engine->drawLine(pos, Vector2f(1.0f, 0.0f), width, bColor);
	pos.x += width;
	engine->drawLine(pos, Vector2f(0.0f, -1.0f), 32, bColor);
	engine->drawText(shownText, font, 32, 32, 16, textColor, position.x + 5, position.y + 5);
}

void TextBox::checkKeyPressed() {
	int keyPressed = 0;

	if (engine->isAsciiKeyPressed(KEY_BACKSPACE)) {
		keyPressed = KEY_BACKSPACE;
	} else if (engine->isAsciiKeyPressed(KEY_DECIMAL)) {
		keyPressed = KEY_DECIMAL;
	} else {
		for (char i = 32; i <= 126; ++i) {
			if (engine->isAsciiKeyPressed(i)) {
				keyPressed = i;
				break;
			}
		}
	}

	if (keyPressed != 0) {
		switch (keyPressed) {
			case KEY_UP:
			case KEY_DOWN:
				break;
			case KEY_LEFT:
			case KEY_RIGHT:
				arrowKeyPressed(keyPressed);
				break;
			case KEY_BACKSPACE:
				backspaceKeyPressed();
				break;
			case KEY_DELETE:
				deleteKeyPressed();
				break;
			case KEY_DECIMAL:
				keyPressed = '.';
			default:
				charKeyPressed(keyPressed);
		}
		discardKeyPressTimer = false;
		keyPressTimer.startTimer();
	} else {
		discardKeyPressTimer = true;
	}
}

void TextBox::charKeyPressed(char character) {
	if (character >= 65 && character <= 90 && !engine->isAsciiKeyPressed(KEY_SHIFT)) {
		character += 32;
	}

	if (text.size() < maxLength) {
		if (cursorPosition == text.size()) {
			text.push_back(character);
		} else {
			text.insert(text.begin() + cursorPosition, character);
		}
		++cursorPosition;
		updateCharOffsetAndShownText();
	}
}

void TextBox::arrowKeyPressed(char val) {
	if (val == KEY_LEFT) {
		if (cursorPosition > 0) {
			--cursorPosition;
			updateCharOffsetAndShownText();
		}
	} else {
		if (cursorPosition < text.size()) {
			++cursorPosition;
			updateCharOffsetAndShownText();
		}
	}
}

void TextBox::deleteKeyPressed() {
	if (cursorPosition < text.size()) {
		text.erase(cursorPosition, 1);
		updateCharOffsetAndShownText();
	}
}

void TextBox::backspaceKeyPressed() {
	if (cursorPosition > 0) {
		text.erase(cursorPosition - 1, 1);
		--cursorPosition;
		updateCharOffsetAndShownText();
	}
}

void TextBox::updateCharOffsetAndShownText() {
	if (text.size() < maxShownCharacters) {
		shownText = text;
		charOffset = 0;
	} else {
		if (cursorPosition - charOffset >= maxShownCharacters) {
			++charOffset;
		} else if (cursorPosition - charOffset < 0) {
			--charOffset;
		}
		shownText = text.substr(charOffset, maxShownCharacters);
	}
}

std::string TextBox::getText() {
	return text;
}

void TextBox::setText(std::string text) {
	this->text = text;
	cursorPosition = 0;
	charOffset = 0;
	shownText = text.substr(charOffset, maxShownCharacters);
}