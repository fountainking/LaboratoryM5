#ifndef MUSIC_TOOLS_H
#define MUSIC_TOOLS_H

#include <M5Cardputer.h>

// Music tools state
enum MusicToolsState {
  TOOLS_MENU,
  GUITAR_TUNER,
  AUDIO_VISUALIZER,
  LAB_BEAT_MACHINE,
  TAP_TEMPO
};

// Guitar tuner functions
void enterMusicTools();
void drawMusicToolsMenu();
void startGuitarTuner();
void drawGuitarTuner();
void updateGuitarTuner();
void stopGuitarTuner();

// Audio visualizer functions
void startAudioVisualizer();
void drawAudioVisualizer();
void updateAudioVisualizer();
void stopAudioVisualizer();

// Navigation
void handleMusicToolsNavigation(char key);

// Global state
extern MusicToolsState musicToolsState;
extern int musicToolsMenuIndex;

#endif
