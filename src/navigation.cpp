#include "navigation.h"
#include "ui.h"
#include "wifi_manager.h"
#include "wifi_fun.h"
#include "file_manager.h"
#include "wifi_transfer.h"
#include "settings.h"
#include "terminal.h"
#include "star_rain.h"
#include "roadmap.h"
#include "the_book.h"
#include "radio.h"
#include "music_player.h"
#include "music_tools.h"
#include <AnimatedGIF.h>

// GIF objects for star animations
AnimatedGIF starGif;
bool gifInitialized = false;
bool starGifOpen = false;
bool starGifPlaying = false;
String starGifToPlay = "";
int starGifFrameCount = 0;
int starGifXOffset = 0;
int starGifYOffset = 0;

// Custom GIF draw function for star animations (positioned at bottom center)
// 50% scaling - draws 32x32 GIFs at 16x16 (skip every other pixel)
void StarGIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *usPalette, usTemp[320];
  int x, y, iWidth;

  iWidth = pDraw->iWidth;
  if (iWidth > 240) iWidth = 240;

  usPalette = pDraw->pPalette;

  // Apply custom offset for star positioning with 50% scaling
  // Only draw every other line (skip odd lines for 50% vertical scale)
  if ((pDraw->iY + pDraw->y) % 2 != 0) return;

  x = pDraw->iX / 2 + starGifXOffset;
  y = (pDraw->iY + pDraw->y) / 2 + starGifYOffset;

  if (y >= 135 || x >= 240) return; // Off screen

  s = pDraw->pPixels;

  // Handle transparency with 50% horizontal scaling (skip every other pixel)
  if (pDraw->ucHasTransparency) {
    uint8_t c, ucTransparent = pDraw->ucTransparent;
    for (int px = 0; px < iWidth; px += 2) { // Skip every other pixel
      if (s[px] == ucTransparent) {
        continue;
      }
      c = s[px];
      M5Cardputer.Display.drawPixel(x + (px / 2), y, usPalette[c]);
    }
  } else {
    // No transparency - draw every other pixel
    for (int px = 0; px < iWidth; px += 2) {
      M5Cardputer.Display.drawPixel(x + (px / 2), y, usPalette[s[px]]);
    }
  }
}

void drawStillStar() {
  // Initialize SD if not done yet
  if (!gifInitialized) {
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
    SD.begin(SD_SPI_CS_PIN, SPI, SD_SPI_FREQ);
    gifInitialized = true;
  }

  // Initialize GIF with Little Endian palette
  starGif.begin(GIF_PALETTE_RGB565_LE);

  // Determine which star to show based on current state
  const char* starFile = "/Gifs/starL2.gif"; // Use left star as default still
  if (currentState == APPS_MENU) {
    starFile = "/Gifs/starL2.gif";
  } else if (currentState == MAIN_MENU) {
    starFile = "/Gifs/starL2.gif";
  }

  // Try to open and draw just the first frame
  if (starGif.open(starFile, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, StarGIFDraw)) {
    // Get GIF dimensions for centering
    int gifWidth = starGif.getCanvasWidth();
    int gifHeight = starGif.getCanvasHeight();

    // 50% scaling - 32x32 GIF displays as 16x16
    // Center at position 120, 118
    starGifXOffset = 120 - (gifWidth / 4);
    starGifYOffset = 118 - (gifHeight / 4);

    // Draw only first frame (still image)
    starGif.playFrame(false, NULL);
    starGif.close();
  }
}

void startStarGif(const char* filename) {
  // Just set the filename to play - actual playback happens in main loop
  starGifToPlay = String(filename);
}

void updateStarGifPlayback() {
  // Check if we need to start a new animation (can interrupt current one)
  if (starGifToPlay != "") {
    // Stop any currently playing GIF to allow restart
    if (starGifOpen) {
      starGif.close();
      starGifOpen = false;
      starGifPlaying = false;
    }

    // Initialize SD if not done yet
    if (!gifInitialized) {
      SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
      SD.begin(SD_SPI_CS_PIN, SPI, SD_SPI_FREQ);
      gifInitialized = true;
    }

    // Initialize GIF with Little Endian palette
    starGif.begin(GIF_PALETTE_RGB565_LE);

    Serial.printf("Attempting to open GIF: %s\n", starGifToPlay.c_str());

    // Try to open the GIF
    if (starGif.open(starGifToPlay.c_str(), GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, StarGIFDraw)) {
      starGifOpen = true;
      starGifPlaying = true;
      starGifFrameCount = 0;

      // Get GIF dimensions for centering
      int gifWidth = starGif.getCanvasWidth();
      int gifHeight = starGif.getCanvasHeight();

      Serial.printf("GIF opened successfully! Size: %dx%d\n", gifWidth, gifHeight);

      // 50% scaling - 32x32 GIF displays as 16x16
      starGifXOffset = 120 - (gifWidth / 4);
      starGifYOffset = 118 - (gifHeight / 4);

      starGifToPlay = "";  // Clear the request
    } else {
      Serial.println(F("Failed to open GIF file!"));
    }
  }

  // Play one frame per call for non-blocking, interruptible animation
  if (starGifPlaying && starGifOpen) {
    // Determine background color
    bool inverted = (currentState == APPS_MENU);
    uint16_t bgColor = inverted ? TFT_BLACK : TFT_WHITE;

    // Clear star area before drawing next frame to prevent layering
    M5Cardputer.Display.fillRect(starGifXOffset, starGifYOffset, 18, 18, bgColor);

    if (!starGif.playFrame(true, NULL)) {
      // Animation finished
      Serial.printf("Animation complete after %d frames\n", starGifFrameCount);
      starGif.close();
      starGifOpen = false;
      starGifPlaying = false;
      drawStillStar();
      return;
    }

    starGifFrameCount++;

    // Safety limit
    if (starGifFrameCount > 50) {
      Serial.println(F("Hit safety limit - stopping animation"));
      starGif.close();
      starGifOpen = false;
      starGifPlaying = false;
      drawStillStar();
    }
  }
}

void navigateUp() {
  // Same as navigateLeft for most screens
  navigateLeft();
}

void navigateLeft() {
  bool inverted = (currentState == APPS_MENU || currentState == SCREEN_VIEW);  // Apps menu and screen views are inverted (black bg)

  if (currentState == WIFI_SAVED) {
    if (selectedSavedIndex > 0) {
      selectedSavedIndex--;
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
      drawWiFiSaved();
    }
  } else if (currentState == WIFI_SCAN) {
    if (selectedNetworkIndex > 0) {
      selectedNetworkIndex--;
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
      drawWiFiScan();
    }
  } else if (currentState == APPS_MENU) {
    if (currentAppIndex > 0) {
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
      // Change state and draw new screen IMMEDIATELY
      currentAppIndex--;
      drawScreen(inverted);
      // Start animation and play it immediately
      startStarGif("/Gifs/starL2.gif");
      updateStarGifPlayback();
    } else {
      // At first app, go back to screensaver (black background version)
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
      extern bool screensaverActive;
      initStarRain(STARRAIN_SCREENSAVER);
      screensaverActive = true;
    }
  } else if (currentState == MAIN_MENU) {
    if (currentMainIndex > 0) {
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
      // Change state and draw new screen IMMEDIATELY
      currentMainIndex--;
      drawScreen(false);
      // Start animation and play it immediately
      startStarGif("/Gifs/starL2.gif");
      updateStarGifPlayback();
    } else {
      // At first main menu item, go to star rain landing page (white background)
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
      stopStarRain();  // Stop any existing star rain
      initStarRain(STARRAIN_LANDING);
      currentState = STAR_LANDING_PAGE;
    }
  }
}

void navigateRight() {
  bool inverted = (currentState == APPS_MENU || currentState == SCREEN_VIEW);  // Apps menu and screen views are inverted (black bg)
  
  if (currentState == WIFI_SAVED) {
    if (selectedSavedIndex < numSavedNetworks - 1) {
      selectedSavedIndex++;
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
      drawWiFiSaved();
    }
  } else if (currentState == WIFI_SCAN) {
    int totalItems = min(numSavedNetworks, 5) + numNetworks;
    if (selectedNetworkIndex < totalItems - 1) {
      selectedNetworkIndex++;
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
      drawWiFiScan();
    }
  } else if (currentState == APPS_MENU) {
    if (currentAppIndex < totalApps - 1) {
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
      // Change state and draw new screen IMMEDIATELY
      currentAppIndex++;
      drawScreen(inverted);
      // Start animation and play it immediately
      startStarGif("/Gifs/star_R2.gif");
      updateStarGifPlayback();
    }
  } else if (currentState == MAIN_MENU) {
    if (currentMainIndex < totalMainItems - 1) {
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
      // Change state and draw new screen IMMEDIATELY
      currentMainIndex++;
      drawScreen(false);
      // Start animation and play it immediately
      startStarGif("/Gifs/star_R2.gif");
      updateStarGifPlayback();
    }
  }
}

void handleSelect() {
  if (currentState == MAIN_MENU) {
    if (currentMainIndex == 0) {
      // APPS
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
      currentState = APPS_MENU;
      currentAppIndex = 0;
      drawScreen(true);
    } else if (currentMainIndex == 1) {
      // Join Wi-Fi
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
      scanWiFiNetworks();
    } else if (currentMainIndex == 2) {
      // The Book
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
      currentState = SCREEN_VIEW;
      currentScreenNumber = 7;
      enterTheBook();
    } else if (currentMainIndex == 3) {
      // Games
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
      currentState = SCREEN_VIEW;
      currentScreenNumber = 8;
      drawPlaceholderScreen(8, "Games", false);
    } else if (currentMainIndex == 4) {
      // Settings
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
      currentState = SCREEN_VIEW;
      currentScreenNumber = 11;
      enterSettingsApp();
    } else if (currentMainIndex == 5) {
      // About
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
      currentState = SCREEN_VIEW;
      currentScreenNumber = 9;
      enterRoadmap();
    }
  } else if (currentState == APPS_MENU) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
    currentState = SCREEN_VIEW;
    currentScreenNumber = apps[currentAppIndex].screenNumber;
    
    if (currentScreenNumber == 1) {
      enterFileManager();
    } else if (currentScreenNumber == 2) {
      enterWiFiFunApp();
    } else if (currentScreenNumber == 3) {
      enterWiFiTransferApp();
    } else if (currentScreenNumber == 4) {
      enterRadioApp();
    } else if (currentScreenNumber == 5) {
      enterTerminal();
    } else if (currentScreenNumber == 6) {
      enterMusicPlayer();
    } else if (currentScreenNumber == 7) {
      enterTheBook();
    } else if (currentScreenNumber == 12) {
      // Music menu
      extern void drawMusicMenu();
      extern int musicMenuIndex;
      musicMenuIndex = 0;
      drawMusicMenu();
    } else {
      drawPlaceholderScreen(currentScreenNumber, apps[currentAppIndex].name.c_str(), true);
    }
  } else if (currentState == WIFI_SCAN) {
    int numSaved = min(numSavedNetworks, 5);
    
    if (selectedNetworkIndex < numSaved) {
      // Selecting a saved network - connect directly
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
      connectToSavedNetwork(selectedNetworkIndex);
    } else {
      // Selecting a scanned network
      int scannedIndex = selectedNetworkIndex - numSaved;
      if (scannedIndex < numNetworks) {
        if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
        targetSSID = scannedNetworks[scannedIndex];
        
        // Check if already saved
        bool isSaved = false;
        int savedIdx = -1;
        for (int i = 0; i < numSavedNetworks; i++) {
          if (savedSSIDs[i] == targetSSID) {
            isSaved = true;
            savedIdx = i;
            break;
          }
        }
        
        if (isSaved) {
          connectToSavedNetwork(savedIdx);
        } else {
          inputPassword = "";
          currentState = WIFI_PASSWORD;
          drawWiFiPassword();
        }
      }
    }
  } else if (currentState == WIFI_PASSWORD) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
    connectToWiFi();
  } else if (currentState == WIFI_SAVED) {
    if (numSavedNetworks > 0) {
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
      connectToSavedNetwork(selectedSavedIndex);
    }
  }
}

void handleBack() {
  // ALWAYS reset speaker volume first to prevent loud beeps
  M5Cardputer.Speaker.setVolume(80);

  if (currentState == SCREEN_VIEW) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);

    if (currentScreenNumber == 2 && wifiFunState != WIFI_FUN_MENU) {
      return;
    }

    if (currentScreenNumber == 1 && sdCardMounted) {
      return;
    }

    // Route back based on screen number:
    // APPS menu: 1 (Files), 2 (WiFi Fun), 3 (Transfer), 5 (Terminal), 12 (Music menu)
    // MAIN menu: 7 (The Book), 8 (Games), 9 (About), 10 (Join WiFi), 11 (Settings)
    // Music menu: 4 (Radio), 6 (Music Player), 13 (Music Tools)

    if (currentScreenNumber == 1 || currentScreenNumber == 2 || currentScreenNumber == 3 ||
        currentScreenNumber == 5 || currentScreenNumber == 12) {
      // APPS menu screens
      currentState = APPS_MENU;
      drawScreen(true);
    } else if (currentScreenNumber == 4 || currentScreenNumber == 6 || currentScreenNumber == 13) {
      // Music submenu screens - go back to Music menu
      int prevScreen = currentScreenNumber;  // Save before changing
      currentScreenNumber = 12;
      extern int musicMenuIndex;
      if (prevScreen == 4) musicMenuIndex = 1;  // Radio
      else if (prevScreen == 6) musicMenuIndex = 0;  // Player
      else musicMenuIndex = 2;  // Tools
      extern void drawMusicMenu();
      drawMusicMenu();
    } else {
      // MAIN menu screens (7, 8, 9, 10, 11)
      currentState = MAIN_MENU;
      drawScreen(false);
    }
  } else if (currentState == APPS_MENU) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
    currentState = MAIN_MENU;
    drawScreen(false);
  } else if (currentState == WIFI_SCAN) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
    currentState = MAIN_MENU;
    drawScreen(false);
  } else if (currentState == WIFI_PASSWORD) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
    inputPassword = "";
    currentState = WIFI_SCAN;
    drawWiFiScan();
  } else if (currentState == WIFI_SAVED) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
    currentState = MAIN_MENU;
    drawScreen(false);
  }
}