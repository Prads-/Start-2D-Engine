#ifndef AUDIO_CORE_EXCEPTION_H
#define AUDIO_CORE_EXCEPTION_H

#include "CustomException.h"

class AudioCoreException : public CustomException { 
public:
	AudioCoreException(std::string msg) : CustomException(msg) { }
};

#endif