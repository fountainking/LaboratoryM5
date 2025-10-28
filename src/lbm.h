#ifndef LBM_H
#define LBM_H

#include <Arduino.h>

// LBM (Lab Beat Machine) - 8-track step sequencer

// Constants
#define LBM_TRACKS 8
#define LBM_STEPS 16
#define LBM_MIN_BPM 40
#define LBM_MAX_BPM 240

// Track types
enum TrackType {
  TRACK_KICK = 0,
  TRACK_SNARE = 1,
  TRACK_HAT = 2,
  TRACK_TOM = 3,
  TRACK_MELODY1 = 4,
  TRACK_MELODY2 = 5,
  TRACK_MELODY3 = 6,
  TRACK_MELODY4 = 7
};

// Sound modes
enum SoundMode {
  MODE_POLY = 0,    // Polyphonic system beeps
  MODE_808 = 1,     // 808 samples from SD
  MODE_BEATBOX = 2, // User-recorded samples
  MODE_USER = 3     // User-selected MP3s
};

// Pattern data structure
struct Pattern {
  bool steps[LBM_TRACKS][LBM_STEPS];  // Step grid (on/off)
  uint8_t notes[LBM_TRACKS][LBM_STEPS]; // Note values for melody tracks (0-127 MIDI)
  uint8_t nudge[LBM_TRACKS];          // Timing nudge (0-4)
  float speed[LBM_TRACKS];            // Speed multiplier (0.5, 1.0, 2.0)
  int bpm;                            // Tempo
  SoundMode mode;                     // Sound mode
  char name[32];                      // Pattern name
};

// LBM state
extern Pattern currentPattern;
extern int currentTrack;              // 0-7
extern int cursorX;                   // 0-7 (step column)
extern int cursorY;                   // 0-1 (step row)
extern bool isPlaying;
extern unsigned long lastStepTime;
extern int playbackStep;              // Current step being played (0-15)
extern bool lbmActive;

// Track colors (from mockups)
extern uint16_t trackColors[LBM_TRACKS];

// Track names
extern const char* trackNames[LBM_TRACKS];

// LBM functions
void enterLBM();
void exitLBM();
void drawLBM();
void updateLBM();
void handleLBMNavigation(char key);

// Audio functions
void playStep(int track, int step);
void triggerTrack(int track);
void playPOLYSound(TrackType track, uint8_t note);

// Pattern functions
void clearPattern();
void savePattern(const char* filename);
void loadPattern(const char* filename);

#endif
