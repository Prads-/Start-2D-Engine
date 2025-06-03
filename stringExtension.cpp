/*
	Written By Pradipna Nepal
	www.pradsprojects.com

	Copyright (C) 2013 Pradipna Nepal
	Please read COPYING.txt included along with this source code for more detail.
	If not included, see http://www.gnu.org/licenses/
*/

#include "stringExtension.h"
using namespace std;

void StringExtension::strAppendInteger(int integer, string *strOut) {
	string appendStr = intToStr(integer);
	strOut->append(appendStr);
}

std::string StringExtension::intToStr(int integer) {
	if (integer == 0) return "0";

	string retStr = "";
	int digit;
	bool neg = false;
	
	if (integer < 0) {
		neg = true;
		integer = ~integer + 1;
	}
	while (integer != 0) {
		digit = integer % 10;
		retStr += (char)(digit + 48);
		integer /= 10;
	}
	for (int start = 0, end = retStr.size() - 1; start < retStr.size() / 2; ++start, --end) {
		retStr[start] ^= retStr[end];
		retStr[end] ^= retStr[start];
		retStr[start] ^= retStr[end];
	}
	if (neg) retStr.insert(retStr.begin(), '-');

	return retStr;
}

bool StringExtension::getLineFromFile(ifstream *file, string *lineOut) {
	bool retVal = getline(*file, *lineOut);
	if (retVal && lineOut->size() != 0) {
		if (lineOut->at(lineOut->size() - 1) == '\r') {
			lineOut->resize(lineOut->size() - 1);
		}
	}
	return retVal;
}

int StringExtension::strToInt(std::string str) {
	int retVal = 0;

	for (int i = 0; i < str.size(); ++i) {
		if (isdigit(str[i])) {
			retVal = retVal * 10 + (str[i] - 48);
		} else {
			break;
		}
	}

	return retVal;
}

std::string StringExtension::floatToString(float f, uint32 precision) {
	if (f == 0) {
		return "0";
	}

	string retStr = "";
	string intStr = "", fracStr = "", zeros = "";
	bool neg = false;
	if (precision > 6) {
		precision = 6;
	} else if (precision == 0) {
		precision = 1;
	}
	precision = pow(10.0, (double)precision); 
	int integral = f, fractional = (f - integral) * precision, digit;
	
	if (integral < 0) {
		integral = ~integral + 1;
		fractional = ~fractional + 1;
		neg = true;
	}
	
	while (integral != 0) {
		digit = integral % 10;
		intStr += (char)(digit + 48);
		integral /= 10;
	}
	precision /= 10;
	if (fractional != 0) {
		while (precision > fractional) {
			zeros.push_back('0');
			precision /= 10;
		}
		while (fractional != 0) {
			digit = fractional % 10;
			fracStr.push_back((char)(digit + 48));
			fractional /= 10;
		}
	} else {
		while (precision != 0) {
			zeros.push_back('0');
			precision /= 10;
		}
	}
	for (int start = 0, end = intStr.size() - 1; start < intStr.size() / 2; ++start, --end) {
		intStr[start] ^= intStr[end];
		intStr[end] ^= intStr[start];
		intStr[start] ^= intStr[end];
	}
	for (int start = 0, end = fracStr.size() - 1; start < fracStr.size() / 2; ++start, --end) {
		fracStr[start] ^= fracStr[end];
		fracStr[end] ^= fracStr[start];
		fracStr[start] ^= fracStr[end];
	}
	if (neg) {
		retStr = '-';
	}
	retStr.append(intStr);
	retStr.push_back('.');
	retStr.append(zeros);
	retStr.append(fracStr);

	return retStr;
}