#ifndef ARENA_H
#define ARENA_H

#include "Image.h"
#include <string>

class Arena {
private:
	std::string path;
	Image portrait, icon;
	int id;

public:
	Arena(std::string path, int id);

	const std::string *getPath() const;
	const Image *getPortrait() const;
	const Image *getIcon() const;

	int getId() const;
};

#endif