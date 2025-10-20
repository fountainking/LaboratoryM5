#ifndef UI_H
#define UI_H

#include "config.h"

// Utility functions
String getCurrentTime();
int getBatteryPercent();
bool isCharging();

// Drawing functions
void drawStar(int x, int y, int size, float angle, uint16_t fillColor, uint16_t outlineColor);
void drawIndicatorDots(int currentIndex, int totalItems, bool inverted = false);
void drawStatusBar(bool inverted = false);
void drawCard(const char* label, bool inverted = false);
void drawPlaceholderScreen(int screenNum, const char* title, bool inverted = false);
void drawScreen(bool inverted = false);

// WiFi UI functions
void drawWiFiScan();
void drawWiFiPassword();
void drawWiFiSaved();

// Music UI functions
void drawMusicMenu();

#endif