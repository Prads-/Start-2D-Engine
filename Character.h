#ifndef CHARACTER_H
#define CHARACTER_H

#include "Image.h"
#include <string>
#include "CharacterVariables.h"

class Character {
private:
	Image art, avatar;
	std::string name, path;
	AIVariables aiVariables;
	int tokenCounter, id;
	
public:
	Character(std::string path, int id, bool loadImages = true);

	std::string getName() const;
	AIVariables getAIVariables() const;
	PlayerVariables getPlayerVariables() const;

	void setName(std::string name);
	void setAIVariables(AIVariables aiVariables);

	const Image *getArt() const;
	const Image *getAvatar() const;

	std::string getPath() const;

	int getTokenCounter();
	void resetTokenCounter();
	void incrementTokenCounter();

	int getId() const;
};

#endif