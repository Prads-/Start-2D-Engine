#include "AudioCore.h"
#include "AudioCoreException.h"

#ifdef BUILD_FOR_WINDOWS
#include <Windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#include <cstring>
#include "CrossPlatformSleep.h"
using namespace CrossPlatformSleep;

AudioCore::AudioCore() {
	isStreaming = false;
	streamThreadSwitch = false;
	
	alcDevice = alcOpenDevice(0);
	if (alcDevice == 0) {
		throw AudioCoreException("Could not open Audio Device");
	}
	alcContext = alcCreateContext(alcDevice, 0);
	if (alcContext == 0) {
		throw AudioCoreException("Could not create Audio Context");
	}
	if (!alcMakeContextCurrent(alcContext)) {
		throw AudioCoreException("alcMakeContextCurrent failed");
	}
	alGenSources(1, &streamSource);

	alSourcef(streamSource, AL_PITCH, 1.0f);
	alSourcef(streamSource, AL_GAIN, 1.0f);
	alSource3f(streamSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(streamSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alSourcei(streamSource, AL_LOOPING, AL_FALSE);

	resetListener();
}

AudioCore::~AudioCore() {
	stopStreaming();
	while(isStreaming) { 
		crossPlatformSleep(1);
	}
	alDeleteSources(1, &streamSource);
	alcDestroyContext(alcContext);
	alcCloseDevice(alcDevice);
}

void AudioCore::resetListener() {
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_ORIENTATION, 0.0f, 0.0f, -1.0f);
}

void AudioCore::streamingThread() {
	ALuint buffers[2];
	alGenBuffers(2, buffers);

	const int BUFFER_MAX_SIZE = streamOggVorbisFile.vi->rate * 4 * 3;	//3 seconds of playable audio
	uint8 *pcmBuffer = new uint8[BUFFER_MAX_SIZE];
	uint8 tempBuffer[4096];
	memset(tempBuffer, 0, 4096);

	ALenum format = (streamOggVorbisFile.vi->channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	alBufferData(buffers[0], format, tempBuffer, 4096, streamOggVorbisFile.vi->rate);
	alBufferData(buffers[1], format, tempBuffer, 4096, streamOggVorbisFile.vi->rate);
	alSourceQueueBuffers(streamSource, 2, buffers);
	alSourcePlay(streamSource);

	while (streamThreadSwitch) {
		ALint buffersProcessed;
		alGetSourcei(streamSource, AL_BUFFERS_PROCESSED, &buffersProcessed);
		if (buffersProcessed > 0) {
			ALuint processedBuffer;
			alSourceUnqueueBuffers(streamSource, 1, &processedBuffer);
			
			bool eof = false;
			int bufferCounter = 0, bitStream;
			while (bufferCounter < BUFFER_MAX_SIZE) {
				int bytesRead = ov_read(&streamOggVorbisFile, (char*)&pcmBuffer[bufferCounter], BUFFER_MAX_SIZE - bufferCounter, 0, 2, 1, &bitStream);
				if (bytesRead <= 0) {
					eof = true;
					break;
				}
				bufferCounter += bytesRead;
			}
			alBufferData(processedBuffer, format, pcmBuffer, bufferCounter, streamOggVorbisFile.vi->rate);
			alSourceQueueBuffers(streamSource, 1, &processedBuffer);
			ALint playState;
			alGetSourcei(streamSource, AL_SOURCE_STATE, &playState);
			if (playState != AL_PLAYING) {
				alSourcePlay(streamSource);
			}
			if (eof) {
				ALint state = AL_PLAYING;
				while (state == AL_PLAYING) {
					crossPlatformSleep(1);
					alGetSourcei(streamSource, AL_SOURCE_STATE, &state);
				}
				break;
			}
		} else {
			crossPlatformSleep(1);
		}
	}

	alSourceStop(streamSource);
	alSourcei(streamSource, AL_BUFFER, 0);
	alDeleteBuffers(2, buffers);
	delete [] pcmBuffer;
	ov_clear(&streamOggVorbisFile);
	isStreaming = false;
}

void AudioCore::streamThreadFunc(void *obj) {
	((AudioCore*)obj)->streamingThread();
}

void AudioCore::startStreaming(std::string path) {
	if (!isStreaming) {
		int retVal = ov_fopen(path.c_str(), &streamOggVorbisFile);
		if (retVal != 0) {
			return;
		}
		isStreaming = true;
		streamThreadSwitch = true;
#ifdef BUILD_FOR_WINDOWS
		_beginthread(streamThreadFunc, 0, this);
#else
        pthread_t t;
        pthread_create(&t, 0, (void*(*)(void*))streamThreadFunc, (void*)this);
#endif
	}
}

void AudioCore::stopStreaming() {
	streamThreadSwitch = false;
}

bool AudioCore::getIsStreaming() {
	return isStreaming;
}

void AudioCore::setStreamGain(float gain) {
	alSourcef(streamSource, AL_GAIN, gain);
}

void AudioCore::getSource(AudioSource *sourceOut) {
	OggVorbis_File oggVorbisFile;

	alGenSources(1, &sourceOut->source);
	
	alSourcef(sourceOut->source, AL_PITCH, 1.0f);
	alSourcef(sourceOut->source, AL_GAIN, 1.0f);
	alSource3f(sourceOut->source, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(sourceOut->source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alSourcei(sourceOut->source, AL_LOOPING, AL_FALSE);
}

void AudioCore::deleteSource(AudioSource *source) {
	stopSource(source);
	alDeleteSources(1, &source->source);
}

void AudioCore::updateSourcePlayingStatus(AudioSource *source) {
	ALint retVal;
	alGetSourcei(source->source, AL_SOURCE_STATE, &retVal);
	if (retVal != AL_PLAYING) {
		source->isPlaying = false;
	}
}

bool AudioCore::createAudioBuffer(std::string path, AudioCore::AudioBuffer *bufferOut) {
	if (bufferOut->isLoaded) {
		return false;
	}

	OggVorbis_File oggVorbisFile;

	if (ov_fopen(path.c_str(), &oggVorbisFile) != 0) {
		return false;
	}

	ogg_int64_t pcmSize = ov_pcm_total(&oggVorbisFile, -1);
	pcmSize *= 2 * oggVorbisFile.vi->channels;
	uint8 *pcmBuffer = new uint8[pcmSize];

	int bufferCounter = 0, bitStream;
	while (bufferCounter < pcmSize) {
		int bytesRead = ov_read(&oggVorbisFile, (char*)&pcmBuffer[bufferCounter], pcmSize - bufferCounter, 0, 2, 1, &bitStream);
		if (bytesRead <= 0) {
			break;
		}
		bufferCounter += bytesRead;
	}

	alGenBuffers(1, &bufferOut->buffer);
	alBufferData(bufferOut->buffer, (oggVorbisFile.vi->channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, pcmBuffer, pcmSize, oggVorbisFile.vi->rate);
	bufferOut->isLoaded = true;

	delete [] pcmBuffer;
	ov_clear(&oggVorbisFile);
	return true;
}

void AudioCore::bindBufferToSource(AudioCore::AudioSource *source, AudioCore::AudioBuffer *buffer) {
	alSourcei(source->source, AL_BUFFER, buffer->buffer);
	source->attachedBuffer = buffer->buffer;
}

void AudioCore::bindBufferToSource(AudioCore::AudioSource *source, ALuint buffer) {
	alSourcei(source->source, AL_BUFFER, buffer);
	source->attachedBuffer = buffer;
}

void AudioCore::deleteAudioBuffer(AudioCore::AudioBuffer *buffer) {
	alDeleteBuffers(1, &buffer->buffer);
}

void AudioCore::setSourcePosition(const AudioSource *source, Vector2f position) {
	alSource3f(source->source, AL_POSITION, position.x, position.y, 0.0f); 
}

void AudioCore::playSource(AudioSource *source) {
	alSourcePlay(source->source);
	source->isPlaying = true;
}

void AudioCore::stopSource(AudioSource *source) {
	alSourceStop(source->source);
	source->isPlaying = false;
}

void AudioCore::loopSource(const AudioSource *source, bool loop) {
	alSourcei(source->source, AL_LOOPING, loop);
}

void AudioCore::setSourcePitch(const AudioSource *source, float pitch) {
	alSourcef(source->source, AL_PITCH, pitch);
}

void AudioCore::setSourceGain(const AudioSource *source, float gain) {
	alSourcef(source->source, AL_GAIN, gain);
}
