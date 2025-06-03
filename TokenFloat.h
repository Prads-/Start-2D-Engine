#ifndef TOKEN_FLOAT_H
#define TOKEN_FLOAT_H

#include "Token.h"

class TokenFloat : public Token {
private:
	float value;

public:
	int getTokenType() const;

	float getFloatValue() const;
	void setFloatValue(float floatVal);
};

#endif