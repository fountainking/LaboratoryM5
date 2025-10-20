#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <M5Cardputer.h>
#include <AudioOutputI2S.h>
#include <AudioGeneratorMP3.h>
#include <AudioFileSourceSD.h>
#include <AudioFileSourceHTTPStream.h>

// Audio source types
enum AudioSource {
  AUDIO_SOURCE_NONE,
  AUDIO_SOURCE_MUSIC,
  AUDIO_SOURCE_RADIO
};

// Global audio manager functions
void initAudioManager();
AudioOutputI2S* getSharedAudioOutput();
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
