#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <string>
#include <vector>
#include "Token.h"

namespace FileParser {
	std::vector<Token*> parseEnvironmentFileString(std::vector<std::string> *srcLines);
	std::vector<Token*> parseCharacterFileString(std::vector<std::string> *srcLines);
	std::vector<Token*> parseLevelFileString(std::vector<std::string> *srcLines);
}

#endif