#ifndef GAME_EXCEPTION_H
#define GAME_EXCEPTION_H

#include <exception>
#include <string>

class GameException : public std::exception {
private:
	std::string errorMsg;

public:
	GameException(std::string errorMsg) {
		this->errorMsg = errorMsg;
	}

	const char *what() const throw() {
		return errorMsg.c_str();
	}
};

#endif