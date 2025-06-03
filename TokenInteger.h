#ifndef TOKEN_INTEGER_H
#define TOKEN_INTEGER_H

#include "Token.h"

class TokenInteger : public Token {
private:
	int value;

public:
	int getTokenType() const;

	int getIntValue() const;
	void setIntValue(int intVal);
};

#endif