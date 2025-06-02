#include "Input.h"
#include <cstring>

Input::Input(int32 *keys) {
	memcpy(this->keys, keys, INPUT_KEY_MAX * 4);
	for (int i = 0; i < INPUT_KEY_MAX; ++i) {
		keysPreviousState[i] = false;
	}
	mouseX = 0;
	mouseY = 0;
	mouseLeftPressed = false;
	mouseMiddlePressed = false;
	mouseRightPressed = false;
	previousMouseLeftPressed = false;
	previousMouseMiddlePressed = false;
	previousMouseRightPressed = false;

#ifdef BUILD_FOR_WINDOWS
	for (int i = 0; i < 256; ++i) {
		keysStatus[i] = false;
	}
#else
	for (int i = 0; i < GLFW_KEY_LAST; ++i) {
		keysStatus[i] = false;
	}
#endif
}

bool Input::isKeyPressed(uint32 keyCode) {
	if (keyCode < 0 || keyCode >= INPUT_KEY_MAX) return false;

	keysPreviousState[keyCode] = keysStatus[keys[keyCode]];
	return keysPreviousState[keyCode];
}

bool Input::isAsciiKeyPressed(int asciiCode) {
	return keysStatus[asciiCode];
}

bool Input::wasKeyToggled(uint32 keyCode) {
	if (keyCode < 0 || keyCode >= INPUT_KEY_MAX) return false;
	bool currentKeyState;

	currentKeyState = keysStatus[keys[keyCode]];
	bool retVal = keysPreviousState[keyCode] && !currentKeyState;
	keysPreviousState[keyCode] = currentKeyState;
	return retVal;
}

void Input::setKeyStatus(int keyCode, bool isPressed) {
	keysStatus[keyCode] = isPressed;
}

void Input::setMouseXY(int x, int y) {
	if (isCursorInsideBBox(Vector2i(x, y), &drawSpaceBBox)) {
		mouseX = (x - drawSpaceBBox.minP.x) * mXMultiplier;
		mouseY = (y - drawSpaceBBox.minP.y) * mYMultiplier;
	}
}

void Input::setMouseLeftPressed(bool pressed) {
	mouseLeftPressed = pressed;
}

void Input::setMouseMiddlePressed(bool pressed) {
	mouseMiddlePressed = pressed;
}

void Input::setMouseRightPressed(bool pressed) {
	mouseRightPressed = pressed;
}

void Input::getMouseXY(uint16 &x, uint16 &y) {
	x = mouseX;
	y = mouseY;
}

bool Input::isMouseLeftPressed() {
	return mouseLeftPressed;
}

bool Input::isMouseMiddlePressed() {
	return mouseMiddlePressed;
}

bool Input::isMouseRightPressed() {
	return mouseRightPressed;
}

bool Input::wasMouseLeftToggled() {
	bool retVal = previousMouseLeftPressed && !mouseLeftPressed;
	previousMouseLeftPressed = mouseLeftPressed;
	return retVal;
}
bool Input::wasMouseMiddleToggled() {
	bool retVal = previousMouseMiddlePressed && !mouseMiddlePressed;
	previousMouseMiddlePressed = mouseMiddlePressed;
	return retVal;
}

bool Input::wasMouseRightToggled() {
	bool retVal = previousMouseRightPressed && !mouseRightPressed;
	previousMouseRightPressed = mouseRightPressed;
	return retVal;
}

void Input::getMouseMultiplierXY(float &x, float &y) {
    x = mXMultiplier;
    y = mYMultiplier;
}

void Input::setMouseMultiplierXY(float x, float y) {
	this->mXMultiplier = x;
    this->mYMultiplier = y;
}

void Input::setDrawSpaceBBox(BoundingBox bBox) {
	drawSpaceBBox = bBox;
}

bool Input::isCursorInsideBBox(const BoundingBox *bBox) {
	return isCursorInsideBBox(Vector2i(mouseX, mouseY), bBox);
}

bool Input::isCursorInsideBBox(Vector2i cursor, const BoundingBox *bBox) {
	if (cursor.x < bBox->minP.x || cursor.y < bBox->minP.y || cursor.x > bBox->maxP.x || cursor.y > bBox->maxP.y) {
		return false;
	}
	return true;
}

void Input::resetInputToggles() {
	for (int i = 0; i < 8; ++i) {
		keysPreviousState[i] = false;
	}
	previousMouseLeftPressed = false;
	previousMouseMiddlePressed = false;
	previousMouseRightPressed = false;
}