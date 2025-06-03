#include "TokenInteger.h"

int TokenInteger::getTokenType() const {
	return TOKEN_INT_VALUE;
}

int TokenInteger::getIntValue() const {
	return value;
}
	
void TokenInteger::setIntValue(int intVal) {
	value = intVal;
}