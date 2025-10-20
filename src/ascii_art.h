#ifndef ASCII_ART_H
#define ASCII_ART_H

#include <Arduino.h>

// ASCII art fonts and gradients
void cmd_art(const String& args);
void renderAsciiArt(const String& text, const String& font, const String& gradient);
uint16_t getGradientColor(const String& gradient, int position, int total);
uint16_t interpolateColor(uint16_t color1, uint16_t color2, float t);

#endif
