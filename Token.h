#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "Keywords.h"

#define TOKEN_KEYWORD		0
#define TOKEN_INT_VALUE		1
#define TOKEN_BOOL_VALUE	2
#define TOKEN_STR_VALUE		3
#define TOKEN_FLOAT_VALUE	4

class Token {
public:
	virtual ~Token() { }

	virtual int getTokenType() const = 0;
	
	virtual Keyword getKeyword() const;
	virtual int getIntValue() const;
	virtual bool getBoolValue() const;
	virtual std::string getStringValue() const;
	virtual float getFloatValue() const;

	virtual void setKeyword(Keyword keyword);
	virtual void setIntValue(int intVal);
	virtual void setBoolValue(bool boolVal);
	virtual void setStringValue(std::string strVal);
	virtual void setFloatValue(float floatVal);
};

#endif