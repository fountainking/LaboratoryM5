#include "music_player.h"
#include "ui.h"
#include "settings.h"
#include "file_manager.h"

// Music player state - USE char arrays instead of String to prevent heap fragmentation!
static char musicFiles[50][64];  // 50 files, 64 chars max (fixed allocation, no heap fragmentation)
static int musicCount = 0;
static int selectedMusicIndex = 0;
static bool isPlaying = false;
static char lastError[128] = "";  // Store last error message (fixed size)
static char musicFolderPath[16] = "";  // Store which folder we're using (/mp3 or /mp3s)

void loadMusicFolder() {
  musicCount = 0;

  // Check both /mp3 and /mp3s folders (try /mp3 first, then /mp3s)
  const char* musicFolder = nullptr;
  Serial.println("Music Player: Checking for music folders...");
  if (SD.exists("/mp3")) {
    musicFolder = "/mp3";
    Serial.println("  Found /mp3 folder");
  } else if (SD.exists("/mp3s")) {
    musicFolder = "/mp3s";
    Serial.println("  Found /mp3s folder");
  } else {
    Serial.println("  ERROR: No music folder found (/mp3 or /mp3s)");
    return;  // No music folder found
  }

  // Save the folder path for playback
  strncpy(musicFolderPath, musicFolder, 15);
  musicFolderPath[15] = '\0';
  Serial.printf("  Using folder: %s\n", musicFolderPath);

  File dir = SD.open(musicFolder);
  if (!dir || !dir.isDirectory()) {
    Serial.println("  ERROR: Failed to open music folder as directory");
    return;
  }

  // Load all MP3 files from /mp3s folder
  File file = dir.openNextFile();
  while (file && musicCount < 50) {
    const char* filename = file.name();  // No String allocation!

    // Extract just the filename (not full path) - NO String allocations!
    const char* baseFilename = strrchr(filename, '/');
    if (baseFilename) {
      baseFilename++;  // Skip the '/'
    } else {
      baseFilename = filename;
    }

    // Skip hidden files (starting with .) and directories
    int len = strlen(baseFilename);
    if (!file.isDirectory() &&
        len > 4 &&
        (strcasecmp(baseFilename + len - 4, ".mp3") == 0) &&  // Case-insensitive!
        baseFilename[0] != '.') {
      strncpy(musicFiles[musicCount], baseFilename, 63);
      musicFiles[musicCount][63] = '\0';  // Ensure null termination
      musicCount++;
      Serial.printf("Found MP3: %s\n", musicFiles[musicCount-1]);
    }
    file.close();
    file = dir.openNextFile();
  }
  dir.close();

  // Simple alphabetical sort - NO String allocations!
  for (int i = 0; i < musicCount - 1; i++) {
    for (int j = i + 1; j < musicCount; j++) {
      if (strcmp(musicFiles[i], musicFiles[j]) > 0) {
        char temp[64];
        strcpy(temp, musicFiles[i]);
        strcpy(musicFiles[i], musicFiles[j]);
        strcpy(musicFiles[j], temp);
      }
    }
  }
}

void enterMusicPlayer() {
  lastError[0] = '\0';  // Clear error (now char array, not String)
  loadMusicFolder();
  selectedMusicIndex = 0;
  isPlaying = false;

  Serial.printf("Music Player: Loaded %d MP3 files from %s\n", musicCount, musicFolderPath);
  if (musicCount > 0) {
    for (int i = 0; i < musicCount; i++) {
      Serial.printf("  [%d] %s\n", i, musicFiles[i]);  // char array, not String
    }
  }

  drawMusicPlayer();
}

void exitMusicPlayer() {
  // Stop music when exiting the player
  stopAudioPlayback();
}

void drawMusicPlayer() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawStatusBar(true);

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_MAGENTA);
  M5Cardputer.Display.drawString("MUSIC", 80, 25);

  if (musicCount == 0) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString("No MP3 files found", 50, 60);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("Add MP3 files to /mp3", 40, 75);
    M5Cardputer.Display.drawString("or /mp3s on SD card", 45, 85);
  } else {
    // Show currently playing track info (NO String allocations - prevent fragmentation!)
    M5Cardputer.Display.setTextSize(1);
    char displayName[34];  // 30 chars + "..." + null
    strncpy(displayName, musicFiles[selectedMusicIndex], 33);
    displayName[33] = '\0';

    // Remove .mp3 extension if present
    int len = strlen(displayName);
    if (len > 4 && strcmp(displayName + len - 4, ".mp3") == 0) {
      displayName[len - 4] = '\0';
      len -= 4;
    }

    // Truncate to 30 chars with "..."
    if (len > 30) {
      displayName[27] = '.';
      displayName[28] = '.';
      displayName[29] = '.';
      displayName[30] = '\0';
    }

    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString(displayName, 10, 45);

    // Draw play/pause icon
    if (isPlaying && isAudioPlaying()) {
      // Pause icon (two bars)
      M5Cardputer.Display.fillRect(215, 43, 4, 10, TFT_GREEN);
      M5Cardputer.Display.fillRect(221, 43, 4, 10, TFT_GREEN);
    } else {
      // Play icon (triangle)
      M5Cardputer.Display.fillTriangle(215, 43, 215, 53, 225, 48, TFT_DARKGREY);
    }

    // Track counter
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    char trackInfo[20];
    sprintf(trackInfo, "%d/%d", selectedMusicIndex + 1, musicCount);
    M5Cardputer.Display.drawString(trackInfo, 10, 57);

    // Volume indicator
    int vol = getAudioVolume();
    M5Cardputer.Display.setTextColor(TFT_CYAN);
    char volInfo[10];
    sprintf(volInfo, "Vol:%d%%", vol);
    M5Cardputer.Display.drawString(volInfo, 180, 57);

    // Progress bar (simple visual indicator)
    int barWidth = 220;
    int barX = 10;
    int barY = 70;

    // Draw progress bar background
    M5Cardputer.Display.drawRect(barX, barY, barWidth, 8, TFT_DARKGREY);

    // Fill progress if playing
    if (isPlaying && isAudioPlaying()) {
      // Simple animated progress (we don't have actual position, so show activity)
      static int animPos = 0;
      animPos = (animPos + 1) % barWidth;
      int fillWidth = (animPos * barWidth) / barWidth;
      M5Cardputer.Display.fillRect(barX + 1, barY + 1, fillWidth, 6, TFT_MAGENTA);
    }

    // Show mini track list below
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("TRACKS:", 10, 85);

    const int maxVisible = 3;
    int startIndex = selectedMusicIndex - 1;
    if (startIndex < 0) startIndex = 0;
    if (startIndex > musicCount - maxVisible && musicCount > maxVisible)
      startIndex = musicCount - maxVisible;

    int yPos = 95;
    int endIndex = min(startIndex + maxVisible, musicCount);

    for (int i = startIndex; i < endIndex; i++) {
      bool isSelected = (i == selectedMusicIndex);

      M5Cardputer.Display.setTextSize(1);
      M5Cardputer.Display.setTextColor(isSelected ? TFT_WHITE : TFT_DARKGREY);

      // NO String allocations - prevent fragmentation!
      char trackName[30];  // 26 chars + "..." + null
      strncpy(trackName, musicFiles[i], 29);
      trackName[29] = '\0';

      // Remove .mp3 extension if present
      int len = strlen(trackName);
      if (len > 4 && strcmp(trackName + len - 4, ".mp3") == 0) {
        trackName[len - 4] = '\0';
        len -= 4;
      }

      // Truncate to 26 chars with "..."
      if (len > 26) {
        trackName[23] = '.';
        trackName[24] = '.';
        trackName[25] = '.';
        trackName[26] = '\0';
      }

      // Show selection indicator
      if (isSelected) {
        M5Cardputer.Display.drawString(">", 10, yPos);
      }

      M5Cardputer.Display.drawString(trackName, 20, yPos);
      yPos += 10;
    }
  }

  // Show error message if present
  if (strlen(lastError) > 0) {
    M5Cardputer.Display.fillRect(0, 115, 240, 10, TFT_RED);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString(lastError, 10, 116);
  }

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString(",/=Nav +/- =Vol Enter=Play `=Back", 15, 125);
}

void playSelectedTrack() {
  if (musicCount == 0) {
    strncpy(lastError, "No MP3 files", 127);
    lastError[127] = '\0';
    return;
  }

  // Build path without String (avoid heap allocation)
  char path[96];  // folder path + / + 64 char filename + null
  snprintf(path, sizeof(path), "%s/%s", musicFolderPath, musicFiles[selectedMusicIndex]);
  Serial.printf("Music Player: Playing track %d/%d: %s\n", selectedMusicIndex + 1, musicCount, path);

  lastError[0] = '\0';  // Clear previous error
  bool success = playAudioFile(path);

  if (success) {
    isPlaying = true;
    Serial.println("Music Player: Playback started successfully");
  } else {
    isPlaying = false;
    strncpy(lastError, "Playback failed!", 127);
    lastError[127] = '\0';
    Serial.println("Music Player: ERROR - Playback failed to start!");
    Serial.println("Possible causes:");
    Serial.println("  1. SD card not accessible");
    Serial.println("  2. File doesn't exist or corrupt");
    Serial.println("  3. Audio hardware not responding");
    Serial.println("  4. Insufficient memory");
  }

  drawMusicPlayer();
}

void nextTrack() {
  if (musicCount == 0) return;

  selectedMusicIndex = (selectedMusicIndex + 1) % musicCount;

  if (isPlaying) {
    playSelectedTrack();
  } else {
    drawMusicPlayer();
  }
}

void previousTrack() {
  if (musicCount == 0) return;

  selectedMusicIndex = (selectedMusicIndex - 1 + musicCount) % musicCount;

  if (isPlaying) {
    playSelectedTrack();
  } else {
    drawMusicPlayer();
  }
}

void togglePlayPause() {
  if (musicCount == 0) return;

  if (isPlaying && isAudioPlaying()) {
    stopAudioPlayback();
    isPlaying = false;
  } else {
    playSelectedTrack();
  }

  drawMusicPlayer();
}

void handleMusicNavigation(char key) {
  if (key == ',' || key == ';') {
    previousTrack();
  } else if (key == '.' || key == '/') {
    nextTrack();
  }
}
