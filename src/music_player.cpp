#include "music_player.h"
#include "ui.h"
#include "settings.h"
#include "file_manager.h"

// Music player state
static String musicFiles[50];
static int musicCount = 0;
static int selectedMusicIndex = 0;
static bool isPlaying = false;

void loadMusicFolder() {
  musicCount = 0;

  // Check if /mp3s folder exists
  if (!SD.exists("/mp3s")) {
    return;
  }

  File dir = SD.open("/mp3s");
  if (!dir || !dir.isDirectory()) {
    return;
  }

  // Load all MP3 files from /mp3s folder
  File file = dir.openNextFile();
  while (file && musicCount < 50) {
    String filename = String(file.name());
    // Skip hidden files (starting with .)
    if (!file.isDirectory() && filename.endsWith(".mp3") && !filename.startsWith(".")) {
      musicFiles[musicCount] = filename;
      musicCount++;
    }
    file = dir.openNextFile();
  }
  dir.close();

  // Simple alphabetical sort
  for (int i = 0; i < musicCount - 1; i++) {
    for (int j = i + 1; j < musicCount; j++) {
      if (musicFiles[i] > musicFiles[j]) {
        String temp = musicFiles[i];
        musicFiles[i] = musicFiles[j];
        musicFiles[j] = temp;
      }
    }
  }
}

void enterMusicPlayer() {
  loadMusicFolder();
  selectedMusicIndex = 0;
  isPlaying = false;
  drawMusicPlayer();
}

void exitMusicPlayer() {
  // Keep music playing in background when exiting app
  // Don't call stopAudioPlayback() anymore
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
    M5Cardputer.Display.drawString("No MP3 files in /mp3s", 40, 60);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("Add MP3 files to /mp3s", 40, 75);
    M5Cardputer.Display.drawString("folder on SD card", 55, 85);
  } else {
    // Show currently playing track info
    M5Cardputer.Display.setTextSize(1);
    String displayName = musicFiles[selectedMusicIndex];
    if (displayName.endsWith(".mp3")) {
      displayName = displayName.substring(0, displayName.length() - 4);
    }
    if (displayName.length() > 30) {
      displayName = displayName.substring(0, 30) + "...";
    }
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString(displayName.c_str(), 10, 45);

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

      String trackName = musicFiles[i];
      if (trackName.endsWith(".mp3")) {
        trackName = trackName.substring(0, trackName.length() - 4);
      }
      if (trackName.length() > 26) {
        trackName = trackName.substring(0, 26) + "...";
      }

      // Show selection indicator
      if (isSelected) {
        M5Cardputer.Display.drawString(">", 10, yPos);
      }

      M5Cardputer.Display.drawString(trackName.c_str(), 20, yPos);
      yPos += 10;
    }
  }

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString(",/=Nav +/- =Vol Enter=Play `=Back", 15, 125);
}

void playSelectedTrack() {
  if (musicCount == 0) return;

  String path = "/mp3s/" + musicFiles[selectedMusicIndex];
  playAudioFile(path);
  isPlaying = true;
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
