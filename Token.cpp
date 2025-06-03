#include "Token.h"

Keyword Token::getKeyword() const {
	return KW_NONE;
}

int Token::getIntValue() const {
	return 0;
}

bool Token::getBoolValue() const {
	return false;
}

std::string Token::getStringValue() const {
	return "";
}

float Token::getFloatValue() const {
	return 0.0f;
}

void Token::setKeyword(Keyword keyword) { }

void Token::setIntValue(int intVal) { }

void Token::setBoolValue(bool boolVal) { }

void Token::setStringValue(std::string stdVal) { }

void Token::setFloatValue(float floatVal) { }