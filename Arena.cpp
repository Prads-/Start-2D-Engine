#include "Arena.h"

Arena::Arena(std::string path, int id) {
	this->id = id;
	portrait.loadImage("maps/" + path + "/portrait.png");
	icon.loadImage("maps/" + path + "/icon.png");
	this->path = path;
}

const std::string *Arena::getPath() const {
	return &path;
}

const Image *Arena::getPortrait() const {
	return &portrait;
}

const Image *Arena::getIcon() const {
	return &icon;
}

int Arena::getId() const {
	return id;
}