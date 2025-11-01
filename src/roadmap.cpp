#include "roadmap.h"
#include "config.h"
#include "captive_portal.h"
#include <M5Cardputer.h>

// Credits scroll state
float creditsScrollOffset = 0.0;
unsigned long lastScrollTime = 0;
const float SCROLL_SPEED = 0.5;  // Pixels per frame

// Roadmap item structure
struct RoadmapItem {
  const char* text;
  bool completed;
};

// Roadmap checklist - mark completed items with true
RoadmapItem roadmapItems[] = {
  // CORE SYSTEM
  {"CORE SYSTEM", false},
  {"[X] Boot animation", true},
  {"[X] Star rain landing page", true},
  {"[X] Status bar (WiFi/Time/Battery)", true},
  {"[X] Navigation with GIF animations", true},
  {"[X] Settings (Sound/Brightness/Timezone)", true},

  // FILE MANAGER
  {"", false},
  {"FILE MANAGER", false},
  {"[X] Browse SD card folders", true},
  {"[X] View text files", true},
  {"[X] View images (GIF)", true},
  {"[X] Play audio files (MP3/WAV)", true},
  {"[X] Delete files", true},
  {"[X] Rename files", true},
  {"[X] Copy/Move files", true},
  {"[X] Search files", true},
  {"[X] Batch operations", true},

  // WI-FI
  {"", false},
  {"WI-FI FEATURES", false},
  {"[X] Scan & connect to networks", true},
  {"[X] Save networks", true},
  {"[X] Fake WiFi AP", true},
  {"[X] Captive portals (portalDECK)", true},
  {"[X] Custom HTML portals", true},
  {"[X] Rick Roll beacon spam", true},
  {"[ ] Deauth attacks", false},
  {"[ ] Packet sniffing", false},
  {"[ ] WPA handshake capture", false},

  // FILE TRANSFER
  {"", false},
  {"FILE TRANSFER", false},
  {"[X] Web interface upload/download", true},
  {"[ ] FTP server", false},
  {"[ ] Bluetooth file transfer", false},

  // TERMINAL
  {"", false},
  {"TERMINAL", false},
  {"[X] Basic shell commands", true},
  {"[X] File system navigation", true},
  {"[X] WiFi commands", true},
  {"[X] Scrollable output", true},
  {"[X] Star rain command", true},
  {"[X] Script execution", true},
  {"[X] Text editor", true},
  {"[ ] Lightweight SSH client", false},
  {"[ ] Custom encrypted protocol", false},
  {"[ ] GPIO control", false},

  // RADIO
  {"", false},
  {"RADIO", false},
  {"[X] Internet radio streaming", true},
  {"[X] Station presets", true},
  {"[X] Background playback", true},
  {"[ ] FM radio", false},
  {"[ ] Sub-GHz transceiver", false},

  // MUSIC
  {"", false},
  {"MUSIC PLAYER", false},
  {"[X] Play local MP3 files", true},
  {"[X] Playlist navigation", true},
  {"[X] Play/pause controls", true},
  {"[X] Progress indicator", true},
  {"[X] Background playback", true},
  {"[ ] Shuffle/repeat modes", false},
  {"[ ] Visualizer", false},

  // FUTURE IDEAS
  {"", false},
  {"FUTURE IDEAS", false},
  {"[ ] BadUSB attacks", false},
  {"[ ] Rubber Ducky scripts", false},
  {"[ ] QR code scanner", false},
  {"[ ] Bitcoin wallet", false},
  {"[ ] Games (Snake, Tetris)", false},
  {"[ ] Weather app", false},
  {"[ ] GPS tracking", false},
  {"[ ] Morse code trainer", false},
};

const int totalRoadmapItems = sizeof(roadmapItems) / sizeof(roadmapItems[0]);

void enterRoadmap() {
  creditsScrollOffset = 135.0;  // Start from bottom of screen
  lastScrollTime = millis();
  drawRoadmap();
}

void drawRoadmap() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);

  // Star Wars-style credits window (trapezoid perspective)
  // Credits scroll from bottom to top with perspective scaling

  const char* credits[] = {
    "",
    "",
    "",
    "LABORATORY M5",
    "",
    "Created by",
    "",
    "JAMES",
    "",
    "",
    "A portable hacking",
    "& creative tool",
    "",
    "Built with ESP32-S3",
    "",
    "2025",
    "",
    "",
    ""
  };
  const int numLines = sizeof(credits) / sizeof(credits[0]);

  // Draw credits with perspective (larger at bottom, smaller at top)
  for (int i = 0; i < numLines; i++) {
    // Calculate Y position for this line
    float lineY = creditsScrollOffset + (i * 18);

    // Only draw if on screen
    if (lineY > 10 && lineY < 100) {
      // Perspective scale: bigger near bottom (lineY ~100), smaller near top (lineY ~10)
      float scale = 1.0 + ((lineY - 55) / 100.0);  // Range: 0.5 to 1.5
      scale = constrain(scale, 0.6, 2.0);

      // Fade effect: fade out near top
      uint8_t alpha = 255;
      if (lineY < 30) {
        alpha = map((int)lineY, 10, 30, 0, 255);
      }

      // Yellow color with alpha (Star Wars style)
      uint16_t color = TFT_YELLOW;
      if (alpha < 128) {
        color = TFT_ORANGE;  // Darker when fading
      }

      M5Cardputer.Display.setTextColor(color);

      // Text size based on scale
      if (scale > 1.5) {
        M5Cardputer.Display.setTextSize(3);
      } else if (scale > 1.0) {
        M5Cardputer.Display.setTextSize(2);
      } else {
        M5Cardputer.Display.setTextSize(1);
      }

      // Center the text (approximate width calculation)
      String line = credits[i];
      int charWidth = (scale > 1.5) ? 18 : (scale > 1.0) ? 12 : 6;
      int textWidth = line.length() * charWidth;
      int textX = (240 - textWidth) / 2;

      M5Cardputer.Display.drawString(line, textX, (int)lineY);
    }
  }

  // Draw bottom button area (dark grey bar)
  M5Cardputer.Display.fillRect(0, 100, 240, 35, 0x2104);  // Dark grey

  // Portal button
  M5Cardputer.Display.fillRoundRect(45, 105, 150, 20, 8, TFT_RED);
  M5Cardputer.Display.drawRoundRect(45, 105, 150, 20, 8, TFT_WHITE);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_WHITE);
  M5Cardputer.Display.drawString("LAUNCH PORTAL (ENTER)", 55, 110);

  // Back button hint
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("` = Back", 90, 120);
}

void updateCreditsScroll() {
  unsigned long now = millis();
  if (now - lastScrollTime > 30) {  // ~30 FPS
    creditsScrollOffset -= SCROLL_SPEED;

    // Loop credits when they scroll off top
    if (creditsScrollOffset < -200) {
      creditsScrollOffset = 135.0;
    }

    lastScrollTime = now;
    drawRoadmap();
  }
}

void scrollRoadmapUp() {
  // Not used in credits mode
}

void scrollRoadmapDown() {
  // Not used in credits mode
}
