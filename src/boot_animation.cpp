#include "boot_animation.h"
#include "config.h"
#include "settings.h"
#include <SPI.h>

static AnimatedGIF bootGif;
static File bootGifFile;

// GIF callback functions for boot animation
void * BootGIFOpenFile(const char *fname, int32_t *pSize) {
  bootGifFile = SD.open(fname);
  if (bootGifFile) {
    *pSize = bootGifFile.size();
    return (void *)&bootGifFile;
  }
  return NULL;
}

void BootGIFCloseFile(void *pHandle) {
  File *f = static_cast<File *>(pHandle);
  if (f != NULL) f->close();
}

int32_t BootGIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
  int32_t iBytesRead = iLen;
  File *f = static_cast<File *>(pFile->fHandle);
  if ((pFile->iSize - pFile->iPos) < iLen) iBytesRead = pFile->iSize - pFile->iPos;
  if (iBytesRead <= 0) return 0;
  iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
  pFile->iPos = f->position();
  return iBytesRead;
}

int32_t BootGIFSeekFile(GIFFILE *pFile, int32_t iPosition) {
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  return pFile->iPos;
}

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

bool playBootGIF(const char* gifPath) {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  
  bootGif.begin(GIF_PALETTE_RGB565_BE);
  
  if (!bootGif.open(gifPath, BootGIFOpenFile, BootGIFCloseFile, BootGIFReadFile, BootGIFSeekFile, BootGIFDraw)) {
    return false;
  }
  
  // Play frames - most boot GIFs are 30-120 frames
  for (int i = 0; i < 120; i++) {
    int result = bootGif.playFrame(true, NULL);
    if (result <= 0) break;
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
  // Initialize SD card
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  bool sdMounted = SD.begin(SD_SPI_CS_PIN, SPI, SD_SPI_FREQ);
  
  if (!sdMounted) {
    // Quick fallback animation if SD fails
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
    return;
  }
  
  bool animationPlayed = false;
  
  // Try to play GIF first
  if (SD.exists("/gifs/boot.gif")) {
    if (playBootGIF("/gifs/boot.gif")) {
      animationPlayed = true;
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
      return;
    }
  }

  // If GIF didn't play, try BMP sequence
  if (!animationPlayed && SD.exists("/boot")) {
    if (playBootBMPSequence("/boot", 121)) {
      animationPlayed = true;
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
      return;
    }
  }

  // Fallback if nothing played
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