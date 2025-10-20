#ifndef RADIO_H
#define RADIO_H

#include <M5Cardputer.h>
#include <AudioFileSourceHTTPStream.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>

// Radio states
enum RadioState {
  RADIO_STOPPED,
  RADIO_PLAYING,
  RADIO_CONNECTING,
  RADIO_ERROR
};

// Station information
struct RadioStation {
  const char* name;
  const char* url;
  const char* genre;
  int bitrate;
};

// Global state
extern RadioState radioState;
extern int currentStationIndex;
extern int volume;

// Functions
void enterRadioApp();
void exitRadioApp();
void drawRadioUI();
void playStation(int stationIndex);
void stopRadio();
void nextStation();
void previousStation();
void increaseVolume();
void decreaseVolume();
void handleRadioNavigation(char key);
void updateRadioApp();
bool isRadioPlaying();

// Station list (built-in stations)
extern const RadioStation stations[];
extern const int stationCount;

#endif
