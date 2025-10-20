#ifndef BOOT_ANIMATION_H
#define BOOT_ANIMATION_H

#include <M5Cardputer.h>
#include <SD.h>
#include <AnimatedGIF.h>

// Boot animation functions
void playBootAnimation();
bool playBootGIF(const char* gifPath);
bool playBootBMPSequence(const char* folderPath, int frameCount);

#endif