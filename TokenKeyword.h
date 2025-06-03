#ifndef TOKEN_KEYWORD_H
#define TOKEN_KEYWORD_H

#include "Token.h"

class TokenKeyword : public Token {
private:
	Keyword value;

public:
	int getTokenType() const;

	Keyword getKeyword() const;
	void setKeyword(Keyword keyword);
};

#endif