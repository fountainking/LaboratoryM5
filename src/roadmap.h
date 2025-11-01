#ifndef ROADMAP_H
#define ROADMAP_H

#include <Arduino.h>

// Credits state
extern float creditsScrollOffset;
extern unsigned long lastScrollTime;

// Credits functions (About screen)
void enterRoadmap();
void drawRoadmap();
void updateCreditsScroll();
void scrollRoadmapUp();
void scrollRoadmapDown();

#endif
