/*
	Written By Pradipna Nepal
	www.pradsprojects.com

	Copyright (C) 2013 Pradipna Nepal
	Please read COPYING.txt included along with this source code for more detail.
	If not included, see http://www.gnu.org/licenses/
*/

#ifndef STRING_EXTENSION_H
#define STRING_EXTENSION_H

#include <string>
#include <fstream>
#include "declarations.h"

namespace StringExtension {
	void strAppendInteger(int integer, std::string *strOut);
	std::string intToStr(int integer);
	bool getLineFromFile(std::ifstream *file, std::string *lineOut);
	int strToInt(std::string str);
	std::string floatToString(float f, uint32 precision);
}

#endif