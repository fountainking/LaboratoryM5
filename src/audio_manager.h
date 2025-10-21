#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <M5Cardputer.h>
#include "AudioOutputM5Speaker.h"  // Custom output for M5 Speaker
#include <AudioGeneratorMP3.h>
#include <AudioFileSourceSD.h>
#include <AudioFileSourceHTTPStream.h>
#include "AudioGeneratorMP3_PSRAM.h"  // PSRAM-optimized MP3 decoder

// Audio source types
enum AudioSource {
  AUDIO_SOURCE_NONE,
  AUDIO_SOURCE_MUSIC,
  AUDIO_SOURCE_RADIO
};

// Global audio manager functions
void initAudioManager();
AudioOutputM5Speaker* getSharedAudioOutput();
AudioSource getCurrentAudioSource();

// Music player interface
bool startMusicPlayback(const String& path);
void stopMusicPlayback();
bool isMusicPlaying();
void updateMusicPlayback();

// Radio interface
bool startRadioStream(const char* url);
void stopRadioStream();
bool isRadioStreaming();
void updateRadioPlayback();

// Volume control
void setMasterVolume(int vol); // 0-100
int getMasterVolume();
void masterVolumeUp();
void masterVolumeDown();

// Temporary mute for smooth navigation
void temporarilyMuteAudio();
void restoreAudioVolume();

#endif
