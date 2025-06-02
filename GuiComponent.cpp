#include "GuiComponent.h"

GuiComponent::GuiComponent(StartEngine *engine, Vector2i position, int width, int height, bool isAlphaBlended) {
	this->engine = engine;
	this->position = position;
	this->width = width;
	this->height = height;
	this->isAlphaBlended = isAlphaBlended;
	calculateBB();
}

void GuiComponent::setSize(int width, int height) {
	this->width = width;
	this->height = height;
	calculateBB();
}

void GuiComponent::setPosition(Vector2i position) {
	this->position = position;
	calculateBB();
}

void GuiComponent::setIsAlphaBlended(bool isAlphaBlended) {
	this->isAlphaBlended = isAlphaBlended;
}

Vector2i GuiComponent::getPosition() {
	return position;
}

void GuiComponent::calculateBB() {
	bBox.minP = Vector2f(position.x, position.y);
	bBox.maxP = Vector2f(position.x + width, position.y + height);
}