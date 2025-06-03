#ifndef TOKEN_STRING_H
#define TOKEN_STRING_H

#include "Token.h"

class TokenString : public Token {
private:
	std::string value;

public:
	int getTokenType() const;
	
	std::string getStringValue() const;
	void setStringValue(std::string strVal);
};

#endif