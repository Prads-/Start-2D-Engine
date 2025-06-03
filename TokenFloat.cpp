#include "TokenFloat.h"

int TokenFloat::getTokenType() const {
	return TOKEN_FLOAT_VALUE;
}

float TokenFloat::getFloatValue() const {
	return value;
}

void TokenFloat::setFloatValue(float floatVal) {
	value = floatVal;
}