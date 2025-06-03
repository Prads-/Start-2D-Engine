#ifndef FILE_LOADER_EXCEPTION_H
#define FILE_LOADER_EXCEPTION_H

#include "CustomException.h"

class FileLoaderException : public CustomException {
public:
	FileLoaderException(std::string msg) : CustomException(msg) { }
};

#endif