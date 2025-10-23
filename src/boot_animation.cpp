#include "boot_animation.h"
#include "config.h"
#include "settings.h"
#include "embedded_gifs.h"
#include <SPI.h>

static AnimatedGIF bootGif;

// GIF draw callback for boot animation

void BootGIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *usPalette, usTemp[320];
  int x, y, iWidth;

  iWidth = pDraw->iWidth;
  if (iWidth > 240) iWidth = 240;
  
  usPalette = pDraw->pPalette;
  
  // Center the GIF vertically
  int gifHeight = bootGif.getCanvasHeight();
  int yOffset = (135 - gifHeight) / 2;
  if (yOffset < 0) yOffset = 0;
  
  y = pDraw->iY + pDraw->y + yOffset;
  
  if (y >= 135) return; // Off screen
  
  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) {
    for (x = 0; x < iWidth; x++) {
      if (s[x] == pDraw->ucTransparent) {
        s[x] = pDraw->ucBackground;
      }
    }
    pDraw->ucHasTransparency = 0;
  }
  
  if (pDraw->ucHasTransparency) {
    uint8_t c, ucTransparent = pDraw->ucTransparent;
    int x, iCount;
    for (x = 0; x < iWidth; x++) {
      if (s[x] == ucTransparent) {
        continue;
      }
      c = s[x];
      usTemp[0] = usPalette[c];
      iCount = 1;
      while (x + iCount < iWidth && s[x + iCount] == c && s[x + iCount] != ucTransparent) {
        usTemp[iCount] = usPalette[c];
        iCount++;
      }
      M5Cardputer.Display.pushImage(pDraw->iX + x, y, iCount, 1, usTemp);
      x += (iCount - 1);
    }
  } else {
    s = pDraw->pPixels;
    for (x = 0; x < iWidth; x++) {
      usTemp[x] = usPalette[s[x]];
    }
    M5Cardputer.Display.pushImage(pDraw->iX, y, iWidth, 1, usTemp);
  }
}

bool playBootGIF() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);

  bootGif.begin(GIF_PALETTE_RGB565_BE);

  // Open embedded boot GIF from PROGMEM
  if (!bootGif.open((uint8_t*)gif_boot, gif_boot_len, BootGIFDraw)) {
    return false;
  }

  // Play all frames
  while (bootGif.playFrame(true, NULL) > 0) {
    // Keep playing until animation completes
  }

  bootGif.close();
  return true;
}

bool playBootBMPSequence(const char* folderPath, int frameCount) {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  
  // Test if we can open the file - try both naming conventions
  String testFilename1 = String(folderPath) + "/frame0000.bmp";
  String testFilename2 = String(folderPath) + "/boot_0000_Layer 120.bmp";
  
  File testFile = SD.open(testFilename1.c_str());
  bool useFrameNaming = true;
  
  if (!testFile) {
    testFile = SD.open(testFilename2.c_str());
    if (testFile) {
      useFrameNaming = false;
    } else {
      return false;
    }
  }
  testFile.close();
  
  // Play frames
  for (int i = 0; i < frameCount; i++) {
    String filename;
    
    if (useFrameNaming) {
      // frame0000.bmp naming
      filename = String(folderPath) + "/frame";
      if (i < 10) {
        filename += "000" + String(i);
      } else if (i < 100) {
        filename += "00" + String(i);
      } else if (i < 1000) {
        filename += "0" + String(i);
      } else {
        filename += String(i);
      }
      filename += ".bmp";
    } else {
      // boot_0000_Layer XXX.bmp naming
      filename = String(folderPath) + "/boot_";
      if (i < 10) {
        filename += "000" + String(i);
      } else if (i < 100) {
        filename += "00" + String(i);
      } else if (i < 1000) {
        filename += "0" + String(i);
      } else {
        filename += String(i);
      }
      filename += "_Layer " + String(120 - i) + ".bmp";
    }
    
    // Draw the frame
    if (!M5Cardputer.Display.drawBmpFile(filename.c_str(), 0, 0)) {
      continue; // Skip if frame can't load
    }
    
    delay(42); // ~20fps - adjust this to change speed (higher = slower)
  }
  
  return true;
}

void playBootAnimation() {
  // Play embedded boot GIF (no SD card needed!)
  if (playBootGIF()) {
    // Show version after animation (right side)
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString(FIRMWARE_VERSION, 200, 125);
    if (settings.soundEnabled) {
      M5Cardputer.Speaker.tone(1200, 100);
      delay(100);
      M5Cardputer.Speaker.tone(1500, 100);
    }
    delay(500);
  } else {
    // Fallback if GIF failed to play
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setTextColor(TFT_YELLOW);
    M5Cardputer.Display.drawString("M5", 95, 55);

    // Display version in lower left
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString(FIRMWARE_VERSION, 5, 125);

    if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 100);
    delay(300);
  }
}