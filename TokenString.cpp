#include "TokenString.h"

int TokenString::getTokenType() const {
	return TOKEN_STR_VALUE;
}

std::string TokenString::getStringValue() const {
	return value;
}

void TokenString::setStringValue(std::string strVal) {
	value = strVal;
}