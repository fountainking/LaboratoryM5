#include "roadmap.h"
#include "config.h"
#include <M5Cardputer.h>

// Roadmap scroll state
int roadmapScrollOffset = 0;

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
  roadmapScrollOffset = 0;
  drawRoadmap();
}

void drawRoadmap() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);

  // Draw title
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("ROADMAP", 70, 5);

  // Draw scrollable list
  M5Cardputer.Display.setTextSize(1);
  int yPos = 25;
  const int lineHeight = 10;
  const int maxVisible = 10;  // Show 10 lines at a time

  int startIndex = roadmapScrollOffset;
  int endIndex = min(startIndex + maxVisible, totalRoadmapItems);

  for (int i = startIndex; i < endIndex; i++) {
    RoadmapItem& item = roadmapItems[i];

    // Check if this is a section header (all caps, no brackets)
    bool isHeader = (strlen(item.text) > 0 &&
                     item.text[0] != '[' &&
                     item.text[0] != ' ' &&
                     strcmp(item.text, "") != 0);

    // Check if empty line
    bool isEmpty = (strlen(item.text) == 0);

    if (isEmpty) {
      // Just add spacing
      yPos += lineHeight / 2;
    } else if (isHeader) {
      // Draw section header in bold/colored
      M5Cardputer.Display.setTextColor(TFT_BLUE);
      M5Cardputer.Display.drawString(item.text, 10, yPos);
      M5Cardputer.Display.setTextColor(TFT_BLACK);
      yPos += lineHeight;
    } else {
      // Draw checkbox item
      uint16_t textColor = item.completed ? TFT_DARKGREY : TFT_BLACK;
      M5Cardputer.Display.setTextColor(textColor);
      M5Cardputer.Display.drawString(item.text, 15, yPos);
      yPos += lineHeight;
    }
  }

  // Draw scroll indicators
  if (roadmapScrollOffset > 0) {
    // Up arrow
    M5Cardputer.Display.fillTriangle(120, 27, 115, 31, 125, 31, TFT_DARKGREY);
  }

  if (endIndex < totalRoadmapItems) {
    // Down arrow
    M5Cardputer.Display.fillTriangle(120, 128, 115, 124, 125, 124, TFT_DARKGREY);
  }

  // Draw controls
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Fn+,/. = Scroll  ` = Back", 30, 120);
}

void scrollRoadmapUp() {
  if (roadmapScrollOffset > 0) {
    roadmapScrollOffset--;
    drawRoadmap();
  }
}

void scrollRoadmapDown() {
  const int maxVisible = 10;
  if (roadmapScrollOffset < totalRoadmapItems - maxVisible) {
    roadmapScrollOffset++;
    drawRoadmap();
  }
}
