#include "VariableSizeFont.h"
#include <fstream>
#include "CustomException.h"
using namespace std;

VariableSizeFont::VariableSizeFont(std::string folderPath) {
	loadVariableFont(folderPath);
}

VariableSizeFont::VariableSizeFont() {
	for (int i = 0; i < 95; ++i) {
		characters[i] = 0;
	}
}

void VariableSizeFont::loadVariableFont(std::string folderPath) {
	for (int i = 0; i < 95; ++i) {
		string characterPath;
		characterPath.push_back(i + 32);
		characterPath += ".png";
		characters[i] = new Image();
		if (!characters[i]->loadImage(folderPath + "/" + characterPath)) {
			delete characters[i];
			characters[i] = 0;
		}
	}
}

const Image *VariableSizeFont::getFontImage(uint8 character) const {
	character -= 32;
	if (character < 95) {
		return characters[character];
	} else {
		return 0;
	}
}