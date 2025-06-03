#include "TokenKeyword.h"

int TokenKeyword::getTokenType() const {
	return TOKEN_KEYWORD;
}

Keyword TokenKeyword::getKeyword() const {
	return value;
}

void TokenKeyword::setKeyword(Keyword keyword) {
	value = keyword;
}