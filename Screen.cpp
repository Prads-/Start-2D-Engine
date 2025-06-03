#include "Screen.h"

Image Screen::background("Images/Screens/Commons/Background.png");
Image Screen::edgeButtonBackgroundDefault("Images/Screens/Commons/DefaultEdgeButton.png");
Image Screen::edgeButtonBackgroundHighlight("Images/Screens/Commons/HighlightEdgeButton.png");
Image Screen::middleButtonBackgroundDefault("Images/Screens/Commons/DefaultMiddleButton.png");
Image Screen::middleButtonBackgroundHighlight("Images/Screens/Commons/HighlightMiddleButton.png");
Image Screen::middleButtonBackgroundDefault2("Images/Screens/Commons/DefaultMiddleButton2.png");
Image Screen::lockedSprite("Images/Screens/Commons/locked.png");
VariableSizeFont Screen::customFont("Images/CustomFont");

Screen::Screen(StartEngine *engine) {
	this->engine = engine;
}

Screen::MoveOn Screen::getMoveOn() {
	return moveOn;
}

void Screen::resetScreen() {
	moveOn = MOVE_ON_NONE;
}

void *Screen::getUserInput() {
	return 0;
}

void Screen::nextScreen() {
	moveOn = MOVE_ON_NEXT;
}

void Screen::previousScreen() {
	moveOn = MOVE_ON_PREVIOUS;
}

const Image *Screen::getBackground() {
	return &background;
}

const VariableSizeFont *Screen::getCustomFont() {
	return &customFont;
}

const Image *Screen::getEdgeButtonBackgroundDefault() {
	return &edgeButtonBackgroundDefault;
}

const Image *Screen::getEdgeButtonBackgroundHighlight() {
	return &edgeButtonBackgroundHighlight;
}

const Image *Screen::getMiddleButtonBackgroundDefault() {
	return &middleButtonBackgroundDefault;
}

const Image *Screen::getMiddleButtonBackgroundHighlight() {
	return &middleButtonBackgroundHighlight;
}

const Image *Screen::getMiddleButtonBackgroundDefault2() {
	return &middleButtonBackgroundDefault2;
}