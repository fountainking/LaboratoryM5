#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <M5Cardputer.h>
#include <SD.h>

// Music player functions
void enterMusicPlayer();
void exitMusicPlayer();
void drawMusicPlayer();
void handleMusicNavigation(char key);
void loadMusicFolder();
void playSelectedTrack();
void nextTrack();
void previousTrack();
void togglePlayPause();

#endif
