#include "file_manager.h"
#include "ui.h"
#include "settings.h"
#include "audio_manager.h"
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <AnimatedGIF.h>
#include <PNGdec.h>

// Audio playback variables (simplified - now using audio_manager)
String currentAudioPath = "";  // Non-static so main.cpp can access it
static unsigned long audioStartTime = 0;

SPIClass sdSPI(HSPI);

FileManagerState fmState = FM_FOLDER_VIEW;
String currentPath = "/";
FileInfo fileInfoList[50];
int fileCount = 0;
int selectedFileIndex = 0;
bool sdCardMounted = false;
bool showOperationsMenu = false;

// GIF playback variables
static bool gifPlaying = false;
static String currentGifPath = "";
static AnimatedGIF gif;
static File gifFile;
int gifYOffset = 0; // Non-static so main.cpp can use it
static bool fullScreenMode = true; // Always full screen for file viewing

// PNG decoder variables
static PNG png;
static File pngFile;

// File operation clipboard
static String clipboardPath = "";
static String clipboardName = "";
static bool clipboardIsCut = false; // true = cut (move), false = copy

// File search
String searchQuery = "";
bool searchActive = false;

// Batch selection
bool fileSelected[50];  // Selection state for each file
int selectedCount = 0;  // Number of selected files

// Forward declarations for PNG callbacks
void * PNGOpenFile(const char *filename, int32_t *size);

// Helper function to draw navigation hints with yellow rounded rectangle background
void drawNavHint(const char* text, int x, int y) {
  // Measure text width
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  int textWidth = M5Cardputer.Display.textWidth(text);
  int padding = 4;

  // Draw yellow rounded rectangle background
  M5Cardputer.Display.fillRoundRect(x - padding, y - 2, textWidth + (padding * 2), 12, 3, TFT_YELLOW);
  M5Cardputer.Display.drawRoundRect(x - padding, y - 2, textWidth + (padding * 2), 12, 3, TFT_BLACK);

  // Draw black text on yellow background
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString(text, x, y);
}

// Custom status bar for file manager - yellow background with black text
void drawFileManagerStatusBar() {
  uint16_t bgColor = TFT_YELLOW;
  uint16_t fgColor = TFT_BLACK;

  // WiFi status box
  int wifiWidth = 110;
  M5Cardputer.Display.fillRoundRect(5, 5, wifiWidth, 18, 9, bgColor);
  for (int i = 0; i < 2; i++) {
    M5Cardputer.Display.drawRoundRect(5+i, 5+i, wifiWidth-i*2, 18-i*2, 9-i, fgColor);
  }

  bool wifiConnected = (WiFi.status() == WL_CONNECTED);
  if (wifiConnected) {
    String rawSSID = WiFi.SSID();
    String ssid = (rawSSID.length() > 16) ? rawSSID.substring(0, 13) + "..." : rawSSID;
    M5Cardputer.Display.setTextColor(fgColor);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString(ssid.c_str(), 10, 10);
  } else {
    M5Cardputer.Display.setTextColor(fgColor);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString("Off", 10, 10);
  }

  // Time box
  int timeWidth = 110;
  M5Cardputer.Display.fillRoundRect(125, 5, timeWidth, 18, 9, bgColor);
  for (int i = 0; i < 2; i++) {
    M5Cardputer.Display.drawRoundRect(125+i, 5+i, timeWidth-i*2, 18-i*2, 9-i, fgColor);
  }

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[10];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    M5Cardputer.Display.setTextColor(fgColor);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString(timeStr, 130, 10);
  } else {
    M5Cardputer.Display.setTextColor(fgColor);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString("--:--", 130, 10);
  }
}

void PNGCloseFile(void *handle);
int32_t PNGReadFile(PNGFILE *page, uint8_t *buffer, int32_t length);
int32_t PNGSeekFile(PNGFILE *page, int32_t position);
int PNGDraw(PNGDRAW *pDraw);

void enterFileManager() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawFileManagerStatusBar();
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_YELLOW);
  M5Cardputer.Display.drawString("Initializing SD...", 60, 50);
  
  // SD init - Initialize SPI bus first
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  
  // Mount SD with correct pins
  sdCardMounted = SD.begin(SD_SPI_CS_PIN, SPI, SD_SPI_FREQ);
  
  if (!sdCardMounted) {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    drawFileManagerStatusBar();
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("SD Card Error!", 40, 60);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString("Insert SD card and try again", 25, 85);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("Press ` to go back", 60, 110);
    fileCount = 0;
    return;
  }
  
  sdCardMounted = true;
  fmState = FM_FOLDER_VIEW;
  currentPath = "/";
  selectedFileIndex = 0;
  
  loadFolder("/");
}

void loadFolder(String path) {
  fileCount = 0;

  // Clear batch selection
  for (int i = 0; i < 50; i++) {
    fileSelected[i] = false;
  }
  selectedCount = 0;

  // Debug - show what we're trying to open
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawFileManagerStatusBar();
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_YELLOW);
  M5Cardputer.Display.drawString(("Opening: " + path).c_str(), 10, 60);
  delay(500);
  
  File root = SD.open(path);
  
  if (!root) {
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("Failed to open!", 10, 75);
    delay(1500);
    drawFolderView();
    return;
  }
  
  if (!root.isDirectory()) {
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("Not a directory!", 10, 75);
    delay(1500);
    root.close();
    drawFolderView();
    return;
  }
  
  File file = root.openNextFile();
  while (file && fileCount < 50) {
    String fileName = String(file.name());

    // Get just the filename without full path
    int lastSlash = fileName.lastIndexOf('/');
    if (lastSlash >= 0) {
      fileName = fileName.substring(lastSlash + 1);
    }

    // Skip hidden files and system files
    if (fileName.startsWith(".") || fileName == "System Volume Information") {
      file.close();
      file = root.openNextFile();
      continue;
    }

    // Populate FileInfo structure
    fileInfoList[fileCount].name = fileName;
    fileInfoList[fileCount].isDirectory = file.isDirectory();
    fileInfoList[fileCount].size = file.size();
    fileInfoList[fileCount].modified = file.getLastWrite();

    if (file.isDirectory()) {
      fileInfoList[fileCount].type = TYPE_FOLDER;
    } else {
      fileInfoList[fileCount].type = getFileType(fileName);
    }

    fileCount++;
    file.close();
    file = root.openNextFile();
  }
  
  root.close();
  
  drawFolderView();
}

FileType getFileType(String filename) {
  filename.toLowerCase();

  if (filename.endsWith(".pdf")) return TYPE_PDF;
  if (filename.endsWith(".jpg") || filename.endsWith(".jpeg") ||
      filename.endsWith(".png") || filename.endsWith(".bmp")) return TYPE_IMAGE;
  if (filename.endsWith(".gif")) return TYPE_GIF;
  if (filename.endsWith(".mp4") || filename.endsWith(".avi")) return TYPE_VIDEO;
  if (filename.endsWith(".mp3") || filename.endsWith(".wav")) return TYPE_AUDIO;
  if (filename.endsWith(".txt") || filename.endsWith(".md") ||
      filename.endsWith(".csv") || filename.endsWith(".rtf")) return TYPE_TEXT;
  if (filename.endsWith(".obj")) return TYPE_MODEL;

  return TYPE_UNKNOWN;
}

String formatFileSize(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < 1024 * 1024) {
    return String(bytes / 1024) + "KB";
  } else {
    return String(bytes / (1024 * 1024)) + "MB";
  }
}

void drawFolderView() {
  M5Cardputer.Display.fillScreen(TFT_YELLOW);
  drawFileManagerStatusBar();

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Files", 5, 28);

  // Show current path
  String displayPath = currentPath;
  if (displayPath.length() > 32) {
    displayPath = "..." + displayPath.substring(displayPath.length() - 29);
  }
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString(displayPath.c_str(), 5, 38);

  // Show search box if searching
  if (searchActive) {
    M5Cardputer.Display.fillRect(2, 26, 236, 12, TFT_WHITE);
    M5Cardputer.Display.drawRect(2, 26, 236, 12, TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_BLACK);
    String displaySearch = "Search: " + searchQuery;
    if (displaySearch.length() > 38) {
      displaySearch = displaySearch.substring(0, 35) + "...";
    }
    M5Cardputer.Display.drawString(displaySearch.c_str(), 5, 28);
  }

  // Draw column headers
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Name", 15, 45);
  M5Cardputer.Display.drawString("Date", 120, 45);
  M5Cardputer.Display.drawString("Size", 195, 45);

  // Draw vertical column lines
  M5Cardputer.Display.drawLine(13, 42, 13, 120, TFT_DARKGREY);
  M5Cardputer.Display.drawLine(115, 42, 115, 120, TFT_DARKGREY);
  M5Cardputer.Display.drawLine(190, 42, 190, 120, TFT_DARKGREY);

  // Build filtered file list if searching
  int filteredIndices[50];
  int filteredCount = 0;

  if (searchActive && searchQuery.length() > 0) {
    String lowerQuery = searchQuery;
    lowerQuery.toLowerCase();
    for (int i = 0; i < fileCount; i++) {
      String lowerName = fileInfoList[i].name;
      lowerName.toLowerCase();
      if (lowerName.indexOf(lowerQuery) >= 0) {
        filteredIndices[filteredCount++] = i;
      }
    }
  } else {
    // No filter, show all files
    for (int i = 0; i < fileCount; i++) {
      filteredIndices[i] = i;
    }
    filteredCount = fileCount;
  }

  if (filteredCount == 0) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString(searchActive ? "No matches" : "Empty folder", 75, 70);
  } else {
    // Show up to 5 items from filtered list
    int startIdx = max(0, selectedFileIndex - 2);
    int endIdx = min(filteredCount, startIdx + 5);

    for (int i = startIdx; i < endIdx; i++) {
      int fileIdx = filteredIndices[i];  // Map to actual file index
      int yPos = 57 + ((i - startIdx) * 12);

      bool selected = (i == selectedFileIndex);

      if (selected) {
        M5Cardputer.Display.fillRoundRect(2, yPos - 1, 236, 11, 2, TFT_WHITE);
      }

      M5Cardputer.Display.setTextSize(1);

      // Show selection checkbox if file is selected for batch operation
      if (fileSelected[fileIdx] && selectedCount > 0) {
        M5Cardputer.Display.fillRect(2, yPos, 8, 8, TFT_GREEN);
        M5Cardputer.Display.drawRect(2, yPos, 8, 8, TFT_BLACK);
      }

      // File type icon or thumbnail
      String icon = " ";
      uint16_t iconColor = TFT_BLACK;
      bool showThumbnail = false;

      // Check if this is an image file and we should show a thumbnail
      if (fileInfoList[fileIdx].type == TYPE_IMAGE) {
        String thumbnailPath;
        if (currentPath.endsWith("/")) {
          thumbnailPath = currentPath + fileInfoList[fileIdx].name;
        } else {
          thumbnailPath = currentPath + "/" + fileInfoList[fileIdx].name;
        }

        // Try to load and display a small thumbnail (10x10 pixels)
        bool loaded = false;
        String lowerPath = thumbnailPath;
        lowerPath.toLowerCase();

        // Clear the thumbnail area first
        M5Cardputer.Display.fillRect(2, yPos, 10, 10, TFT_YELLOW);

        if (lowerPath.endsWith(".jpg") || lowerPath.endsWith(".jpeg")) {
          // Draw scaled down to 10x10
          loaded = M5Cardputer.Display.drawJpgFile(thumbnailPath.c_str(), 2, yPos, 10, 10);
        } else if (lowerPath.endsWith(".png")) {
          loaded = M5Cardputer.Display.drawPngFile(thumbnailPath.c_str(), 2, yPos, 10, 10);
        } else if (lowerPath.endsWith(".bmp")) {
          loaded = M5Cardputer.Display.drawBmpFile(thumbnailPath.c_str(), 2, yPos, 10, 10);
        }

        if (loaded) {
          showThumbnail = true;
          // Draw border around thumbnail
          M5Cardputer.Display.drawRect(2, yPos, 10, 10, TFT_DARKGREY);
        }
      }

      if (!showThumbnail) {
        // Show icon if thumbnail couldn't be loaded
        switch (fileInfoList[fileIdx].type) {
          case TYPE_FOLDER:  icon = "D"; iconColor = TFT_BLUE; break;
          case TYPE_PDF:     icon = "P"; iconColor = TFT_RED; break;
          case TYPE_IMAGE:   icon = "I"; iconColor = TFT_GREEN; break;
          case TYPE_GIF:     icon = "G"; iconColor = TFT_PURPLE; break;
          case TYPE_VIDEO:   icon = "V"; iconColor = TFT_CYAN; break;
          case TYPE_AUDIO:   icon = "M"; iconColor = TFT_ORANGE; break;
          case TYPE_TEXT:    icon = "T"; iconColor = TFT_DARKGREY; break;
          case TYPE_MODEL:   icon = "3"; iconColor = TFT_MAGENTA; break;
          default:           icon = "?"; iconColor = TFT_DARKGREY; break;
        }

        M5Cardputer.Display.setTextColor(iconColor);
        M5Cardputer.Display.drawString(icon.c_str(), 5, yPos);
      }

      // Filename
      M5Cardputer.Display.setTextColor(TFT_BLACK);
      String displayName = fileInfoList[fileIdx].name;
      if (displayName.length() > 16) {
        displayName = displayName.substring(0, 13) + "...";
      }
      M5Cardputer.Display.drawString(displayName.c_str(), 15, yPos);

      // Date (formatted as MM/DD)
      if (fileInfoList[fileIdx].modified > 0) {
        struct tm timeinfo;
        time_t modTime = fileInfoList[fileIdx].modified;
        localtime_r(&modTime, &timeinfo);
        char dateStr[10];
        sprintf(dateStr, "%02d/%02d", timeinfo.tm_mon + 1, timeinfo.tm_mday);
        M5Cardputer.Display.setTextColor(TFT_DARKGREY);
        M5Cardputer.Display.drawString(dateStr, 120, yPos);
      }

      // File size
      if (!fileInfoList[fileIdx].isDirectory) {
        String sizeStr = formatFileSize(fileInfoList[fileIdx].size);
        M5Cardputer.Display.setTextColor(TFT_DARKGREY);
        M5Cardputer.Display.drawString(sizeStr.c_str(), 195, yPos);
      }
    }

    // Scroll indicators
    if (startIdx > 0) {
      M5Cardputer.Display.fillTriangle(230, 59, 225, 64, 235, 64, TFT_BLACK);
    }
    if (endIdx < filteredCount) {
      M5Cardputer.Display.fillTriangle(230, 110, 225, 105, 235, 105, TFT_BLACK);
    }
  }

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);

  // Show selection count if files are selected for batch operations
  if (selectedCount > 0) {
    String selectionMsg = String(selectedCount) + " selected";
    M5Cardputer.Display.setTextColor(TFT_GREEN);
    M5Cardputer.Display.drawString(selectionMsg.c_str(), 5, 112);
  }
  // Show clipboard status if file is cut/copied
  else if (clipboardPath.length() > 0) {
    String clipboardMsg = clipboardIsCut ? "Move: " : "Copy: ";
    clipboardMsg += clipboardName;
    if (clipboardMsg.length() > 25) {
      clipboardMsg = clipboardMsg.substring(0, 22) + "...";
    }
    M5Cardputer.Display.setTextColor(clipboardIsCut ? TFT_RED : TFT_BLUE);
    M5Cardputer.Display.drawString(clipboardMsg.c_str(), 5, 112);
  }

  // Simple search hint at bottom
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  if (searchActive) {
    drawNavHint("Press Esc to clear search", 5, 122);
  } else {
    drawNavHint("Press → or / to search", 5, 122);
  }
}

void drawFileViewer() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawFileManagerStatusBar();
  
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_WHITE);
  M5Cardputer.Display.drawString("File Viewer", 55, 30);
  
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_CYAN);
  M5Cardputer.Display.drawString("Coming soon!", 75, 70);
  
  M5Cardputer.Display.setTextSize(1);
  drawNavHint("Press ` to go back", 60, 120);
}

void drawTextViewer(const String& path) {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawFileManagerStatusBar();
  
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_WHITE);
  
  // Title
  String title = "Text: " + path;
  if (title.length() > 36) title = title.substring(0, 36) + "...";
  M5Cardputer.Display.drawString(title.c_str(), 6, 26);
  
  // Open file
  File f = SD.open(path.c_str(), FILE_READ);
  int16_t y = 40;
  const int16_t lineH = 10;
  const int16_t maxWChars = 38;
  
  if (!f) {
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("Failed to open file", 6, y);
  } else {
    String line;
    while (f.available() && y < 115) {
      line = f.readStringUntil('\n');
      line.replace("\r", "");
      if ((int)line.length() > maxWChars) line = line.substring(0, maxWChars);
      M5Cardputer.Display.drawString(line.c_str(), 6, y);
      y += lineH;
    }
    f.close();
  }
  
  // Footer
  drawNavHint("Press ` to go back", 60, 122);
}

void drawImageViewer(const String& path) {
  M5Cardputer.Display.fillScreen(TFT_BLACK);

  // Check file exists
  if (!SD.exists(path.c_str())) {
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString("File not found", 60, 55);
    drawNavHint("Press ` to return", 65, 122);
    return;
  }

  // Open file and display
  File imgFile = SD.open(path.c_str());
  if (!imgFile) {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString("Cannot open file", 55, 50);
    drawNavHint("Press ` to return", 65, 122);
    return;
  }

  bool success = false;
  String lowerPath = path;
  lowerPath.toLowerCase();

  if (lowerPath.endsWith(".jpg") || lowerPath.endsWith(".jpeg")) {
    success = M5Cardputer.Display.drawJpg(&imgFile, 0, 0, 240, 135);
    imgFile.close();
  } else if (lowerPath.endsWith(".png")) {
    imgFile.close();

    // Get file size for diagnostics
    File sizeCheck = SD.open(path.c_str());
    size_t fileSize = sizeCheck.size();
    sizeCheck.close();

    Serial.printf("PNG file size: %d bytes (%.2f MB)\n", fileSize, fileSize / 1048576.0);

    // For files > 500KB, skip PNGdec and go straight to M5GFX (handles large files better)
    if (fileSize > 512000) {
      Serial.println(F("Large PNG detected, using M5GFX with auto-scaling..."));
      File pngFile = SD.open(path.c_str());
      if (pngFile) {
        M5Cardputer.Display.fillScreen(TFT_BLACK);
        M5Cardputer.Display.setTextColor(TFT_YELLOW);
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.drawString("Loading large image...", 50, 60);

        // Show file size on screen
        String sizeInfo = String(fileSize / 1024) + " KB";
        M5Cardputer.Display.drawString(sizeInfo, 90, 75);
        delay(1500);  // Give user time to see the message

        M5Cardputer.Display.fillScreen(TFT_BLACK);

        // Try M5GFX drawPng with scaling
        Serial.println(F("Calling M5Cardputer.Display.drawPng()..."));
        success = M5Cardputer.Display.drawPng(&pngFile, 0, 0, 240, 135);
        pngFile.close();

        Serial.printf("M5GFX drawPng returned: %s\n", success ? "SUCCESS" : "FAILED");

        if (success) {
          Serial.println(F("PNG displayed successfully!"));
          // Keep image on screen - don't clear
        } else {
          Serial.println(F("M5GFX drawPng failed - PNG may be too complex or unsupported color mode"));
        }
      } else {
        Serial.println(F("Failed to open PNG file"));
        success = false;
      }
    } else {
      // Small file - try PNGdec first for better quality
      int rc = png.open(path.c_str(), PNGOpenFile, PNGCloseFile, PNGReadFile, PNGSeekFile, PNGDraw);
      Serial.printf("PNGdec open result: %d\n", rc);

      if (rc == PNG_SUCCESS) {
        // PNGdec can handle this file
        M5Cardputer.Display.fillScreen(TFT_BLACK);
        rc = png.decode(NULL, 0);
        png.close();
        success = (rc == PNG_SUCCESS);
        Serial.printf("PNGdec decode result: %d\n", rc);
      } else if (rc == 7) {
        // Error 7 = PNG_TOO_BIG - try M5GFX as fallback
        Serial.println(F("PNGdec says too big, trying M5GFX..."));
        File pngFile = SD.open(path.c_str());
        if (pngFile) {
          M5Cardputer.Display.fillScreen(TFT_BLACK);
          success = M5Cardputer.Display.drawPng(&pngFile, 0, 0, 240, 135);
          pngFile.close();
        }
      } else {
        // PNGdec error - try M5GFX as fallback
        Serial.printf("PNGdec failed with error %d, trying M5GFX...\n", rc);
        File pngFile = SD.open(path.c_str());
        if (pngFile) {
          M5Cardputer.Display.fillScreen(TFT_BLACK);
          success = M5Cardputer.Display.drawPng(&pngFile, 0, 0, 240, 135);
          pngFile.close();
        }
      }
    }

    if (!success) {
      // Both methods failed - try one more thing: load without scaling
      Serial.println(F("Standard methods failed, trying drawPng without scaling..."));
      File pngFile = SD.open(path.c_str());
      if (pngFile) {
        M5Cardputer.Display.fillScreen(TFT_BLACK);
        success = M5Cardputer.Display.drawPng(&pngFile, 0, 0);  // No width/height = no scaling
        pngFile.close();

        if (success) {
          Serial.println(F("Success with no-scaling method!"));
        } else {
          Serial.println(F("No-scaling method also failed"));
        }
      }
    }

    if (!success) {
      // All methods failed - show helpful message
      Serial.println(F("All PNG methods failed"));

      M5Cardputer.Display.fillScreen(TFT_BLACK);
      M5Cardputer.Display.setTextSize(1);
      M5Cardputer.Display.setTextColor(TFT_RED);
      M5Cardputer.Display.drawString("PNG format unsupported", 40, 25);

      M5Cardputer.Display.setTextColor(TFT_YELLOW);
      String sizeStr = "Size: " + String(fileSize / 1024) + " KB";
      M5Cardputer.Display.drawString(sizeStr, 75, 42);

      M5Cardputer.Display.setTextColor(TFT_WHITE);
      M5Cardputer.Display.drawString("This PNG uses features", 45, 60);
      M5Cardputer.Display.drawString("not supported by the", 50, 72);
      M5Cardputer.Display.drawString("embedded decoder.", 60, 84);

      M5Cardputer.Display.setTextColor(TFT_CYAN);
      M5Cardputer.Display.drawString("Use WiFi Transfer's", 55, 100);
      M5Cardputer.Display.drawString("auto-optimizer to fix", 50, 112);

      drawNavHint("` Back", 95, 127);
    }
  } else if (lowerPath.endsWith(".bmp")) {
    success = M5Cardputer.Display.drawBmp(&imgFile, 0, 0, 240, 135);
    imgFile.close();
  }

  if (!success) {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString("Failed to display", 55, 50);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString("Image may be corrupted", 40, 65);
    M5Cardputer.Display.drawString("or unsupported format", 40, 75);
  }

  // Show footer
  drawNavHint("Press ` to return", 65, 122);
}

// GIF callback functions
void * GIFOpenFile(const char *fname, int32_t *pSize) {
  gifFile = SD.open(fname);
  if (gifFile) {
    *pSize = gifFile.size();
    return (void *)&gifFile;
  }
  return NULL;
}

void GIFCloseFile(void *pHandle) {
  File *f = static_cast<File *>(pHandle);
  if (f != NULL) f->close();
}

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
  int32_t iBytesRead = iLen;
  File *f = static_cast<File *>(pFile->fHandle);
  if ((pFile->iSize - pFile->iPos) < iLen) iBytesRead = pFile->iSize - pFile->iPos;
  if (iBytesRead <= 0) return 0;
  iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
  pFile->iPos = f->position();
  return iBytesRead;
}

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition) {
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  return pFile->iPos;
}

void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *d, *usPalette, usTemp[320];
  int x, y, iWidth;

  iWidth = pDraw->iWidth;
  if (iWidth > 240) iWidth = 240;
  
  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y + gifYOffset; // Use calculated offset
  
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

// PNG callback functions
void * PNGOpenFile(const char *filename, int32_t *size) {
  pngFile = SD.open(filename);
  if (pngFile) {
    *size = pngFile.size();
    return (void *)&pngFile;
  }
  return NULL;
}

void PNGCloseFile(void *handle) {
  File *f = static_cast<File *>(handle);
  if (f != NULL) f->close();
}

int32_t PNGReadFile(PNGFILE *page, uint8_t *buffer, int32_t length) {
  if (!pngFile) return 0;
  return pngFile.read(buffer, length);
}

int32_t PNGSeekFile(PNGFILE *page, int32_t position) {
  if (!pngFile) return 0;
  return pngFile.seek(position);
}

int PNGDraw(PNGDRAW *pDraw) {
  uint16_t usPixels[240];  // Line buffer for RGB565 pixels

  // Convert the line to RGB565 format
  png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_LITTLE_ENDIAN, 0x00000000);

  // Center the PNG if it's smaller than the display
  int xOffset = 0;
  int yOffset = 0;

  if (pDraw->iWidth < 240) {
    xOffset = (240 - pDraw->iWidth) / 2;
  }
  if (png.getHeight() < 135) {
    yOffset = (135 - png.getHeight()) / 2;
  }

  int y = pDraw->y + yOffset;
  if (y >= 135) return 1; // Off screen

  M5Cardputer.Display.pushImage(xOffset, y, pDraw->iWidth, 1, usPixels);
  return 1; // Success
}

void drawGifViewer(const String& path) {
  gifPlaying = true;
  currentGifPath = path;
  
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  
  gif.begin(GIF_PALETTE_RGB565_BE);
  
  if (gif.open(path.c_str(), GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
    // Get GIF dimensions to center it on full screen
    int gifWidth = gif.getCanvasWidth();
    int gifHeight = gif.getCanvasHeight();
    
    // Calculate vertical centering for full 135px height
    gifYOffset = (135 - gifHeight) / 2;
    if (gifYOffset < 0) gifYOffset = 0;
    
    M5Cardputer.Display.fillScreen(TFT_BLACK);
  } else {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("Failed to load GIF", 50, 60);
    gifPlaying = false;
  }
}

void updateGifPlayback() {
  if (!gifPlaying) return;
  
  int result = gif.playFrame(true, NULL);
  
  if (result == 0) {
    // End of GIF, loop it
    gif.reset();
  } else if (result < 0) {
    // Error occurred
    gifPlaying = false;
  }
  
  // Redraw footer to keep it visible
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  drawNavHint("Press ` to stop", 70, 122);
}

void stopGifPlayback() {
  if (gifPlaying) {
    gif.close();
  }
  gifPlaying = false;
  currentGifPath = "";
}

bool isGifPlaying() {
  return gifPlaying;
}

void drawPDFViewer(const String& path) {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawFileManagerStatusBar();

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_YELLOW);
  M5Cardputer.Display.drawString("PDF Viewer", 55, 50);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_WHITE);
  M5Cardputer.Display.drawString("PDF rendering not yet", 45, 75);
  M5Cardputer.Display.drawString("implemented", 75, 90);

  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  drawNavHint("Press ` to go back", 60, 122);
}

// Audio player functions (now use centralized audio_manager)
void setAudioVolume(int vol) {
  setMasterVolume(vol);
}

int getAudioVolume() {
  return getMasterVolume();
}

void audioVolumeUp() {
  masterVolumeUp();
}

void audioVolumeDown() {
  masterVolumeDown();
}

bool playAudioFile(const String& path) {
  // ALWAYS set currentAudioPath so we can retry with Enter key if it fails
  currentAudioPath = path;
  audioStartTime = millis();

  bool success = startMusicPlayback(path);
  return success;
}

void stopAudioPlayback() {
  stopMusicPlayback();
  currentAudioPath = "";
}

bool isAudioPlaying() {
  return isMusicPlaying();
}

void updateAudioPlayback() {
  updateMusicPlayback();
}

void drawAudioPlayer(const String& path) {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawFileManagerStatusBar();

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_ORANGE);
  M5Cardputer.Display.drawString("Audio Player", 50, 30);

  // Show filename
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_WHITE);
  String filename = path;
  int lastSlash = filename.lastIndexOf('/');
  if (lastSlash >= 0) {
    filename = filename.substring(lastSlash + 1);
  }
  if (filename.length() > 35) {
    filename = filename.substring(0, 35) + "...";
  }
  M5Cardputer.Display.drawString(filename.c_str(), 10, 55);

  // Play status
  M5Cardputer.Display.setTextSize(1);
  if (isAudioPlaying()) {
    M5Cardputer.Display.setTextColor(TFT_GREEN);
    M5Cardputer.Display.drawString("Playing...", 85, 70);

    // Animated equalizer bars
    int barHeight[] = {3, 5, 7, 5, 3};
    for (int i = 0; i < 5; i++) {
      int animHeight = barHeight[i] + ((millis() / 100 + i) % 4);
      M5Cardputer.Display.fillRect(80 + (i * 6), 85 - animHeight, 4, animHeight, TFT_GREEN);
    }

    // Playback time
    unsigned long playTime = (millis() - audioStartTime) / 1000;
    int minutes = playTime / 60;
    int seconds = playTime % 60;
    char timeStr[10];
    sprintf(timeStr, "%02d:%02d", minutes, seconds);
    M5Cardputer.Display.drawString(timeStr, 95, 95);
  } else {
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("Stopped", 90, 75);
  }

  // Controls
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  drawNavHint(";/.=Prev/Next `=Back", 50, 110);
  drawNavHint("Enter=Play/Stop", 65, 122);
}

void showDeleteConfirmation() {
  if (fileCount == 0 || selectedFileIndex >= fileCount) return;

  // Draw confirmation dialog - white with black outline
  M5Cardputer.Display.fillRect(20, 40, 200, 60, TFT_WHITE);
  M5Cardputer.Display.drawRect(20, 40, 200, 60, TFT_BLACK);
  M5Cardputer.Display.drawRect(21, 41, 198, 58, TFT_BLACK);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Delete this file?", 55, 50);

  // Show filename
  String displayName = fileInfoList[selectedFileIndex].name;
  if (displayName.length() > 28) {
    displayName = displayName.substring(0, 25) + "...";
  }
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString(displayName.c_str(), 30, 65);

  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  drawNavHint("Enter=Yes  `=No", 60, 82);

  // Wait for confirmation
  while (true) {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
      if (M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();

        // Check for Enter key via status flag
        if (status.enter) {
          deleteCurrentFile();
          return;
        }

        // Check for backtick in word keys
        for (auto key : status.word) {
          if (key == '`') {
            // Cancelled
            drawFolderView();
            return;
          }
        }
      }
    }
    delay(10);
  }
}

void cutFile() {
  if (fileCount == 0 || selectedFileIndex >= fileCount) return;

  // Store file info for cut operation
  if (currentPath.endsWith("/")) {
    clipboardPath = currentPath + fileInfoList[selectedFileIndex].name;
  } else {
    clipboardPath = currentPath + "/" + fileInfoList[selectedFileIndex].name;
  }
  clipboardName = fileInfoList[selectedFileIndex].name;
  clipboardIsCut = true;

  if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
  drawFolderView();
}

void copyFile() {
  if (fileCount == 0 || selectedFileIndex >= fileCount) return;

  // Store file info for copy operation
  if (currentPath.endsWith("/")) {
    clipboardPath = currentPath + fileInfoList[selectedFileIndex].name;
  } else {
    clipboardPath = currentPath + "/" + fileInfoList[selectedFileIndex].name;
  }
  clipboardName = fileInfoList[selectedFileIndex].name;
  clipboardIsCut = false;

  if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 50);
  drawFolderView();
}

void pasteFile() {
  if (clipboardPath.length() == 0) return;

  // Build destination path
  String destPath;
  if (currentPath.endsWith("/")) {
    destPath = currentPath + clipboardName;
  } else {
    destPath = currentPath + "/" + clipboardName;
  }

  // Check if source and destination are the same
  if (clipboardPath == destPath) {
    M5Cardputer.Display.fillRect(0, 110, 240, 15, TFT_YELLOW);
    M5Cardputer.Display.setTextColor(TFT_BLACK);
    M5Cardputer.Display.drawString("Already here!", 75, 112);
    delay(1000);
    drawFolderView();
    return;
  }

  bool success = false;

  if (clipboardIsCut) {
    // Move operation (rename)
    success = SD.rename(clipboardPath.c_str(), destPath.c_str());
    if (success) {
      clipboardPath = "";
      clipboardName = "";
    }
  } else {
    // Copy operation (read source, write destination)
    File sourceFile = SD.open(clipboardPath.c_str(), FILE_READ);
    if (sourceFile && !sourceFile.isDirectory()) {
      File destFile = SD.open(destPath.c_str(), FILE_WRITE);
      if (destFile) {
        uint8_t buf[512];
        while (sourceFile.available()) {
          int bytesRead = sourceFile.read(buf, sizeof(buf));
          destFile.write(buf, bytesRead);
        }
        destFile.close();
        success = true;
      }
      sourceFile.close();
    }
  }

  if (success) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(1500, 100);
    loadFolder(currentPath);
  } else {
    M5Cardputer.Display.fillRect(0, 110, 240, 15, TFT_YELLOW);
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("Paste failed!", 75, 112);
    delay(1000);
    drawFolderView();
  }
}

void deleteCurrentFile() {
  if (fileCount == 0 || selectedFileIndex >= fileCount) return;

  String filePath;
  if (currentPath.endsWith("/")) {
    filePath = currentPath + fileInfoList[selectedFileIndex].name;
  } else {
    filePath = currentPath + "/" + fileInfoList[selectedFileIndex].name;
  }

  bool success = false;
  if (fileInfoList[selectedFileIndex].isDirectory) {
    success = SD.rmdir(filePath.c_str());
  } else {
    success = SD.remove(filePath.c_str());
  }

  if (success) {
    // Reload folder
    if (selectedFileIndex >= fileCount - 1 && selectedFileIndex > 0) {
      selectedFileIndex--;
    }
    loadFolder(currentPath);
  } else {
    // Show error briefly
    M5Cardputer.Display.fillRect(0, 110, 240, 15, TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("Delete failed!", 70, 112);
    delay(1000);
    drawFolderView();
  }
}

void showBatchDeleteConfirmation() {
  if (selectedCount == 0) return;

  // Draw confirmation dialog - white with black outline
  M5Cardputer.Display.fillRect(20, 40, 200, 60, TFT_WHITE);
  M5Cardputer.Display.drawRect(20, 40, 200, 60, TFT_BLACK);
  M5Cardputer.Display.drawRect(21, 41, 198, 58, TFT_BLACK);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Delete selected files?", 45, 50);

  // Show count
  String countMsg = String(selectedCount) + " files selected";
  M5Cardputer.Display.setTextColor(TFT_RED);
  M5Cardputer.Display.drawString(countMsg.c_str(), 55, 65);

  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  drawNavHint("Enter=Yes  `=No", 60, 82);

  // Wait for confirmation
  while (true) {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
      if (M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();

        // Check for Enter key via status flag
        if (status.enter) {
          batchDeleteFiles();
          return;
        }

        // Check for backtick in word keys
        for (auto key : status.word) {
          if (key == '`') {
            // Cancelled
            drawFolderView();
            return;
          }
        }
      }
    }
    delay(10);
  }
}

void batchDeleteFiles() {
  int deletedCount = 0;
  int failedCount = 0;

  // Delete all selected files
  for (int i = 0; i < fileCount; i++) {
    if (fileSelected[i]) {
      String filePath;
      if (currentPath.endsWith("/")) {
        filePath = currentPath + fileInfoList[i].name;
      } else {
        filePath = currentPath + "/" + fileInfoList[i].name;
      }

      bool success = false;
      if (fileInfoList[i].isDirectory) {
        success = SD.rmdir(filePath.c_str());
      } else {
        success = SD.remove(filePath.c_str());
      }

      if (success) {
        deletedCount++;
      } else {
        failedCount++;
      }
    }
  }

  // Show result
  if (deletedCount > 0) {
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(1500, 100);
  }

  // Reload folder to show changes
  if (selectedFileIndex >= fileCount - deletedCount && selectedFileIndex > 0) {
    selectedFileIndex = 0;
  }
  loadFolder(currentPath);

  // Show brief summary if there were failures
  if (failedCount > 0) {
    M5Cardputer.Display.fillRect(0, 110, 240, 15, TFT_YELLOW);
    M5Cardputer.Display.setTextColor(TFT_RED);
    String failMsg = String(failedCount) + " failed to delete";
    M5Cardputer.Display.drawString(failMsg.c_str(), 55, 112);
    delay(1500);
    drawFolderView();
  }
}

void renameFile() {
  if (fileCount == 0 || selectedFileIndex >= fileCount) return;

  String oldName = fileInfoList[selectedFileIndex].name;
  String newName = oldName;

  // Draw rename dialog
  M5Cardputer.Display.fillRect(10, 35, 220, 70, TFT_WHITE);
  M5Cardputer.Display.drawRect(10, 35, 220, 70, TFT_BLACK);
  M5Cardputer.Display.drawRect(11, 36, 218, 68, TFT_BLACK);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Rename:", 20, 45);

  // Input loop
  bool cursorVisible = true;
  unsigned long lastBlink = millis();

  while (true) {
    // Draw input field with cursor
    M5Cardputer.Display.fillRect(20, 60, 200, 12, TFT_WHITE);
    M5Cardputer.Display.setTextColor(TFT_BLACK);

    String displayName = newName;
    if (displayName.length() > 28) {
      displayName = displayName.substring(displayName.length() - 28);
    }
    M5Cardputer.Display.drawString(displayName.c_str(), 20, 60);

    // Draw cursor
    if (cursorVisible) {
      int cursorX = 20 + (displayName.length() * 6);
      M5Cardputer.Display.drawLine(cursorX, 60, cursorX, 70, TFT_BLACK);
    }

    // Blink cursor
    if (millis() - lastBlink > 500) {
      cursorVisible = !cursorVisible;
      lastBlink = millis();
    }

    // Footer
    M5Cardputer.Display.fillRect(20, 85, 200, 10, TFT_WHITE);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    drawNavHint("Enter=OK  `=Cancel", 50, 85);

    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange()) {
      if (M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();

        // Check for Enter key
        if (status.enter) {
          if (newName.length() > 0 && newName != oldName) {
            // Perform rename
            String oldPath;
            String newPath;

            if (currentPath.endsWith("/")) {
              oldPath = currentPath + oldName;
              newPath = currentPath + newName;
            } else {
              oldPath = currentPath + "/" + oldName;
              newPath = currentPath + "/" + newName;
            }

            if (SD.rename(oldPath.c_str(), newPath.c_str())) {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1500, 100);
              loadFolder(currentPath);
            } else {
              M5Cardputer.Display.fillRect(0, 110, 240, 15, TFT_YELLOW);
              M5Cardputer.Display.setTextColor(TFT_RED);
              M5Cardputer.Display.drawString("Rename failed!", 70, 112);
              delay(1000);
              drawFolderView();
            }
          } else {
            drawFolderView();
          }
          return;
        }

        // Check for Delete key
        if (status.del) {
          if (newName.length() > 0) {
            newName.remove(newName.length() - 1);
          }
          cursorVisible = true;
          lastBlink = millis();
        }

        // Check for character input
        for (auto key : status.word) {
          if (key == '`') {
            // Cancel
            drawFolderView();
            return;
          } else if (key >= 32 && key <= 126 && newName.length() < 50) {
            newName += (char)key;
            cursorVisible = true;
            lastBlink = millis();
          }
        }
      }
    }

    delay(10);
  }
}

void createFolder() {
  String folderName = "";

  // Draw create folder dialog
  M5Cardputer.Display.fillRect(10, 35, 220, 70, TFT_WHITE);
  M5Cardputer.Display.drawRect(10, 35, 220, 70, TFT_BLACK);
  M5Cardputer.Display.drawRect(11, 36, 218, 68, TFT_BLACK);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("New Folder:", 20, 45);

  // Input loop
  bool cursorVisible = true;
  unsigned long lastBlink = millis();

  while (true) {
    // Draw input field with cursor
    M5Cardputer.Display.fillRect(20, 60, 200, 12, TFT_WHITE);
    M5Cardputer.Display.setTextColor(TFT_BLACK);

    String displayName = folderName;
    if (displayName.length() > 28) {
      displayName = displayName.substring(displayName.length() - 28);
    }
    M5Cardputer.Display.drawString(displayName.c_str(), 20, 60);

    // Draw cursor
    if (cursorVisible) {
      int cursorX = 20 + (displayName.length() * 6);
      M5Cardputer.Display.drawLine(cursorX, 60, cursorX, 70, TFT_BLACK);
    }

    // Blink cursor
    if (millis() - lastBlink > 500) {
      cursorVisible = !cursorVisible;
      lastBlink = millis();
    }

    // Footer
    M5Cardputer.Display.fillRect(20, 85, 200, 10, TFT_WHITE);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    drawNavHint("Enter=Create  `=Cancel", 45, 85);

    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange()) {
      if (M5Cardputer.Keyboard.isPressed()) {
        auto status = M5Cardputer.Keyboard.keysState();

        // Check for Enter key
        if (status.enter) {
          if (folderName.length() > 0) {
            // Create folder
            String newFolderPath;
            if (currentPath.endsWith("/")) {
              newFolderPath = currentPath + folderName;
            } else {
              newFolderPath = currentPath + "/" + folderName;
            }

            if (SD.mkdir(newFolderPath.c_str())) {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1500, 100);
              loadFolder(currentPath);
            } else {
              M5Cardputer.Display.fillRect(0, 110, 240, 15, TFT_YELLOW);
              M5Cardputer.Display.setTextColor(TFT_RED);
              M5Cardputer.Display.drawString("Create failed!", 70, 112);
              delay(1000);
              drawFolderView();
            }
          } else {
            drawFolderView();
          }
          return;
        }

        // Check for Delete key
        if (status.del) {
          if (folderName.length() > 0) {
            folderName.remove(folderName.length() - 1);
          }
          cursorVisible = true;
          lastBlink = millis();
        }

        // Check for character input
        for (auto key : status.word) {
          if (key == '`') {
            // Cancel
            drawFolderView();
            return;
          } else if (key >= 32 && key <= 126 && folderName.length() < 50) {
            folderName += (char)key;
            cursorVisible = true;
            lastBlink = millis();
          }
        }
      }
    }

    delay(10);
  }
}

void loadFile(String path) {
  FileType type = getFileType(path);

  if (type == TYPE_TEXT) {
    fmState = FM_FILE_VIEWER;
    drawTextViewer(path);
  } else if (type == TYPE_IMAGE) {
    fmState = FM_FILE_VIEWER;
    drawImageViewer(path);
  } else if (type == TYPE_GIF) {
    fmState = FM_FILE_VIEWER;
    drawGifViewer(path);
  } else if (type == TYPE_AUDIO) {
    fmState = FM_FILE_VIEWER;
    playAudioFile(path);
    drawAudioPlayer(path);
  } else if (type == TYPE_PDF) {
    fmState = FM_FILE_VIEWER;
    drawPDFViewer(path);
  } else {
    // Other file types - placeholder
    fmState = FM_FILE_VIEWER;
    drawFileViewer();
  }
}

void handleFileManagerNavigation(char key) {
  if (fmState == FM_FOLDER_VIEW) {
    if (key == ';') {
      if (selectedFileIndex > 0) {
        selectedFileIndex--;
        if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
        drawFolderView();
      }
    } else if (key == '.') {
      if (selectedFileIndex < fileCount - 1) {
        selectedFileIndex++;
        if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
        drawFolderView();
      }
    }
  } else if (fmState == FM_FILE_VIEWER) {
    // Navigate to previous/next file while viewing
    if (key == ';') {
      if (selectedFileIndex > 0) {
        stopGifPlayback();
        selectedFileIndex--;
        if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);

        // Load the new file
        String filePath;
        if (currentPath.endsWith("/")) {
          filePath = currentPath + fileInfoList[selectedFileIndex].name;
        } else {
          filePath = currentPath + "/" + fileInfoList[selectedFileIndex].name;
        }

        // Skip folders
        if (fileInfoList[selectedFileIndex].type == TYPE_FOLDER) {
          return;
        }

        loadFile(filePath);
      }
    } else if (key == '.') {
      if (selectedFileIndex < fileCount - 1) {
        stopGifPlayback();
        selectedFileIndex++;
        if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);

        // Load the new file
        String filePath;
        if (currentPath.endsWith("/")) {
          filePath = currentPath + fileInfoList[selectedFileIndex].name;
        } else {
          filePath = currentPath + "/" + fileInfoList[selectedFileIndex].name;
        }

        // Skip folders
        if (fileInfoList[selectedFileIndex].type == TYPE_FOLDER) {
          return;
        }

        loadFile(filePath);
      }
    }
  }
}