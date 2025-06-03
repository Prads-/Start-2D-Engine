#ifndef AUDIO_CORE_H
#define AUDIO_CORE_H

#include "build.h"
#include "declarations.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <vorbis/vorbisfile.h>
#include "vector2.h"

class AudioCore {
private:
	ALCdevice *alcDevice;
	ALCcontext *alcContext;
	ALuint streamSource;
	OggVorbis_File streamOggVorbisFile;
	volatile bool streamThreadSwitch, isStreaming;

	void streamingThread();
	static void streamThreadFunc(void *obj);

public:
	struct AudioSource {
		ALuint source, attachedBuffer;
		bool isPlaying;

		AudioSource() {
			isPlaying = false;
		}
	};

	struct AudioBuffer {
	private:
		bool isLoaded;
		friend class AudioCore;
	public:
		ALuint buffer;
		uint32 lastBoundSourceId;
		bool loop;
		float gain, pitch;

		AudioBuffer() {
			isLoaded = false;
			loop = false;
			gain = 1.0f;
			pitch = 1.0f;
		}
	};

	AudioCore();
	~AudioCore();

	void resetListener();

	void startStreaming(std::string path);
	void stopStreaming();
	bool getIsStreaming();
	void setStreamGain(float gain);

	void getSource(AudioSource *sourceOut);
	void deleteSource(AudioSource *source);
	void updateSourcePlayingStatus(AudioSource *source);

	bool createAudioBuffer(std::string path, AudioCore::AudioBuffer *bufferOut);
	void bindBufferToSource(AudioCore::AudioSource *source, AudioCore::AudioBuffer *buffer);
	void bindBufferToSource(AudioCore::AudioSource *source, ALuint buffer);
	void deleteAudioBuffer(AudioCore::AudioBuffer *buffer);
	
	void setSourcePosition(const AudioSource *source, Vector2f position);
	void playSource(AudioSource *source);
	void stopSource(AudioSource *source);
	void loopSource(const AudioSource *source, bool loop);
	void setSourcePitch(const AudioSource *source, float pitch);
	void setSourceGain(const AudioSource *source, float gain);
};

#endif
