#include "TokenBool.h"

int TokenBool::getTokenType() const {
	return TOKEN_BOOL_VALUE;
}

bool TokenBool::getBoolValue() const {
	return value;
}

void TokenBool::setBoolValue(bool boolVal) {
	value = boolVal;
}