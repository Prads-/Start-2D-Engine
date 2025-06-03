#include "Character.h"
#include "FileLoader.h"
#include "AIObject.h"
#include "PlayerObject.h"
using namespace std;
using namespace FileLoader;

Character::Character(std::string path, int id, bool loadImages) {
	this->id = id;
	loadCharacterParameters("characters/" + path + "/parameters.txt", this);
	
	if (loadImages) {
		art.loadImage("characters/" + path + "/art.png");
		avatar.loadImage("characters/" + path + "/icon.png");
	}
	this->path = path;
	resetTokenCounter();
}

std::string Character::getName() const {
	return name;
}

AIVariables Character::getAIVariables() const {
	return aiVariables;
}

PlayerVariables Character::getPlayerVariables() const {
	PlayerVariables playerVariables;

	playerVariables.height = aiVariables.height;
	playerVariables.width = aiVariables.width;
	playerVariables.maxSpeed = aiVariables.maxSpeed;
	playerVariables.specialThrow = aiVariables.specialThrow;
	playerVariables.secondaryAbility = aiVariables.secondaryAbility;
	
	return playerVariables;
}

void Character::setName(std::string name) {
	this->name = name;
}

void Character::setAIVariables(AIVariables aiVariables) {
	this->aiVariables = aiVariables;
}

const Image *Character::getArt() const {
	return &art;
}

const Image *Character::getAvatar() const {
	return &avatar;
}

std::string Character::getPath() const {
	return path;
}

int Character::getTokenCounter() {
	return tokenCounter;
}

void Character::resetTokenCounter() {
	tokenCounter = 0;
}

void Character::incrementTokenCounter() {
	++tokenCounter;
}

int Character::getId() const {
	return id;
}