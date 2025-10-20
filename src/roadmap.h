#ifndef ROADMAP_H
#define ROADMAP_H

#include <Arduino.h>

// Roadmap state
extern int roadmapScrollOffset;

// Roadmap functions
void enterRoadmap();
void drawRoadmap();
void scrollRoadmapUp();
void scrollRoadmapDown();

#endif
