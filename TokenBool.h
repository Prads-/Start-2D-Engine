#ifndef TOKEN_BOOL_H
#define TOKEN_BOOL_H

#include "Token.h"

class TokenBool : public Token {
private:
	bool value;

public:
	int getTokenType() const;

	bool getBoolValue() const;
	void setBoolValue(bool boolVal);
};

#endif