#ifndef VARIABLE_SIZE_FONT_H
#define VARIABLE_SIZE_FONT_H

#include "Image.h"
#include <string>

class VariableSizeFont {
private:
	Image *characters[95];

public:
	VariableSizeFont(std::string folderPath);
	VariableSizeFont();

	void loadVariableFont(std::string folderPath);
	const Image *getFontImage(uint8 character) const;
};

#endif