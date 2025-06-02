#ifndef CUSTOM_EXCEPTION_H
#define CUSTOM_EXCEPTION_H

#include <exception>
#include <string>

class CustomException : public std::exception {
private:
	std::string message;
	
public:
	CustomException() {
		message = "";
	}

	CustomException(std::string msg) {
		message = msg;
	}

	~CustomException() throw() { }

	void appendMessage(std::string msg) {
		message.append(msg);
	}

	const char *what() const throw() {
		return message.c_str();
	}
};

#endif