#include <M5Cardputer.h>
#include <WiFi.h>
#include <Preferences.h>
#include "config.h"
#include "ui.h"
#include "navigation.h"
#include "wifi_manager.h"
#include "wifi_fun.h"
#include "captive_portal.h"
#include "portal_manager.h"
#include "wifi_transfer.h"
#include "file_manager.h"
#include "boot_animation.h"
#include "settings.h"
#include "background_services.h"
#include "terminal.h"
#include "star_rain.h"
#include "roadmap.h"
#include "radio.h"
#include "music_player.h"
#include "audio_manager.h"
#include "the_book.h"
#include "music_tools.h"

Preferences preferences;

// Define all global variables (declared as extern in config.h)
AppInfo apps[] = {
  {"Files", TFT_YELLOW, 1},
  {"Fun", TFT_BLUE, 2},
  {"Transfer", TFT_CYAN, 3},
  {"Terminal", TFT_GREEN, 5},
  {"Music", TFT_MAGENTA, 12}
};

MainItemInfo mainItems[] = {
  {"APPS", TFT_YELLOW, 0},
  {"Join Wi-Fi", TFT_CYAN, 10},
  {"The Book", TFT_ORANGE, 7},
  {"Games", TFT_PURPLE, 8},
  {"Settings", TFT_DARKGREY, 11},
  {"About", TFT_BLUE, 9}
};

const int totalApps = 5;
const int totalMainItems = 6;

MenuState currentState = MAIN_MENU;
int currentMainIndex = 0;
int currentAppIndex = 0;
int currentScreenNumber = 0;
float starAngle = 0;

String scannedNetworks[20];
int scannedRSSI[20];
int numNetworks = 0;
int selectedNetworkIndex = 0;
String targetSSID = "";
String inputPassword = "";
String wifiSSID = "Not Connected";
bool wifiConnected = false;

// Time globals
bool timeIsSynced = false;
unsigned long lastSyncTime = 0;

// Saved networks
String savedSSIDs[MAX_SAVED_NETWORKS];
String savedPasswords[MAX_SAVED_NETWORKS];
int numSavedNetworks = 0;
int selectedSavedIndex = 0;

// Music submenu
struct MusicMenuItem {
  String name;
  uint16_t color;
  int screenNumber;
};

MusicMenuItem musicMenuItems[] = {
  {"Player", TFT_MAGENTA, 6},
  {"Radio", TFT_RED, 4},
  {"Tools", TFT_CYAN, 13}  // Placeholder for future
};

int totalMusicItems = 3;
int musicMenuIndex = 0;

// Global state for background WiFi connection
unsigned long wifiConnectStartTime = 0;
bool wifiConnecting = false;

// Screensaver state
unsigned long lastActivityTime = 0;
const unsigned long SCREENSAVER_TIMEOUT = 120000; // 2 minutes
bool screensaverActive = false;

// Audio mute during navigation (to avoid hearing skips)
unsigned long lastKeyPressTime = 0;
const unsigned long AUDIO_UNMUTE_DELAY = 300; // Unmute after 300ms of no input
bool audioCurrentlyMuted = false;

// Terminal star rain request
bool terminalStarRainRequested = false;
bool terminalStarRainActive = false;

// Auto-connect WiFi flag
bool autoConnectAttempted = false;
unsigned long autoConnectTriggerTime = 0;

// Skip expensive WiFi operations during boot animation
bool skipWiFiOpsUntilReady = true;
unsigned long bootCompleteTime = 0;

// Flying through space starfield animation
struct Star {
  float x, y, z; // z for depth (distance from viewer)
  uint8_t brightness;
  uint8_t maxBrightness;
  uint8_t type; // 0=dot, 1=small cross, 2=large cross
  uint8_t baseColor; // 0=white, 1=blue, 2=cyan, 3=green, 4=purple, 5=yellow
  int8_t twinkleDirection; // -1=dimming, 1=brightening
  uint8_t twinkleSpeed;
  bool isStatic; // true = slow twinkling static star, false = flying star
};

Star stars[150]; // Even more stars!
bool starsInitialized = false;
float starfieldSpeed = 1.5; // Gradually increases for warp effect
int starfieldFrames = 0; // Count frames to increase speed

void initStarfield() {
  for (int i = 0; i < 150; i++) {
    stars[i].x = random(-120, 120); // Center origin
    stars[i].y = random(-67, 67);

    // 30% static stars, 70% flying stars
    stars[i].isStatic = (i < 45); // First 45 are static

    if (stars[i].isStatic) {
      stars[i].z = random(50, 90); // Static stars in mid-distance
      stars[i].maxBrightness = random(200, 255); // Brighter static stars
    } else {
      stars[i].z = random(10, 100); // Flying stars at all depths
      stars[i].maxBrightness = random(180, 255); // Brighter overall
    }

    stars[i].brightness = random(150, stars[i].maxBrightness);
    stars[i].type = random(0, 10) > 5 ? random(1, 3) : 0; // 50% crosses

    // More colorful distribution
    int colorChoice = random(0, 100);
    if (colorChoice < 40) {
      stars[i].baseColor = 0; // 40% white
    } else if (colorChoice < 60) {
      stars[i].baseColor = 1; // 20% blue
    } else if (colorChoice < 75) {
      stars[i].baseColor = 2; // 15% cyan
    } else if (colorChoice < 85) {
      stars[i].baseColor = 3; // 10% green
    } else if (colorChoice < 95) {
      stars[i].baseColor = 4; // 10% purple/pink
    } else {
      stars[i].baseColor = 5; // 5% yellow/orange
    }

    stars[i].twinkleDirection = random(0, 2) ? 1 : -1;
    stars[i].twinkleSpeed = random(3, 10); // More dramatic twinkle
  }
  starsInitialized = true;
}

void drawStarfield(String statusText = "") {
  if (!starsInitialized) {
    initStarfield();
    starfieldSpeed = 1.5;
    starfieldFrames = 0;
  }

  // Gradually increase speed every frame (faster acceleration!)
  starfieldFrames++;
  starfieldSpeed += 0.15; // Faster acceleration
  if (starfieldSpeed > 12.0) starfieldSpeed = 12.0; // Higher max warp speed

  // Dark blue background
  M5Cardputer.Display.fillScreen(M5Cardputer.Display.color565(0, 0, 25));

  // Draw and update each star
  for (int i = 0; i < 150; i++) {
    // Store old position for streak effect
    float oldZ = stars[i].z;

    // Move star towards viewer (only if not static)
    if (!stars[i].isStatic) {
      stars[i].z -= starfieldSpeed;

      // Reset star to far distance when it gets too close
      if (stars[i].z <= 1) {
        stars[i].x = random(-120, 120);
        stars[i].y = random(-67, 67);
        stars[i].z = 100;
        stars[i].brightness = random(150, stars[i].maxBrightness);
      }
    } else {
      // Static stars move very slowly (drift effect)
      stars[i].z -= 0.1;
      if (stars[i].z <= 40) stars[i].z = 90;
    }

    // Update brightness (dramatic twinkle)
    stars[i].brightness += stars[i].twinkleDirection * stars[i].twinkleSpeed;

    if (stars[i].brightness >= stars[i].maxBrightness) {
      stars[i].brightness = stars[i].maxBrightness;
      stars[i].twinkleDirection = -1;
    } else if (stars[i].brightness <= 120) {
      stars[i].brightness = 120;
      stars[i].twinkleDirection = 1;
    }

    // Project 3D position to 2D screen (perspective)
    int screenX = (int)((stars[i].x / stars[i].z) * 100) + 120; // Center at 120
    int screenY = (int)((stars[i].y / stars[i].z) * 100) + 67;  // Center at 67

    // Skip if off screen
    if (screenX < 0 || screenX >= 240 || screenY < 0 || screenY >= 135) continue;

    // Size increases as star gets closer
    int starSize = (int)(100.0 / stars[i].z);
    if (starSize < 1) starSize = 1;

    // Brightness increases as star gets closer - MUCH BRIGHTER
    uint8_t depthBrightness = (uint8_t)(stars[i].brightness * (100.0 - stars[i].z) / 80.0);
    if (depthBrightness > 255) depthBrightness = 255;
    if (depthBrightness < 100) depthBrightness = 100;

    // Calculate color with brightness - MORE VIBRANT COLORS
    uint16_t color;
    if (stars[i].baseColor == 0) {
      // White - brightest
      color = M5Cardputer.Display.color565(depthBrightness, depthBrightness, depthBrightness);
    } else if (stars[i].baseColor == 1) {
      // Blue - vibrant
      color = M5Cardputer.Display.color565(depthBrightness/4, depthBrightness/2, depthBrightness);
    } else if (stars[i].baseColor == 2) {
      // Cyan - bright
      color = M5Cardputer.Display.color565(0, depthBrightness, depthBrightness);
    } else if (stars[i].baseColor == 3) {
      // Green - emerald
      color = M5Cardputer.Display.color565(0, depthBrightness, depthBrightness/3);
    } else if (stars[i].baseColor == 4) {
      // Purple/Pink - vibrant
      color = M5Cardputer.Display.color565(depthBrightness, depthBrightness/4, depthBrightness);
    } else {
      // Yellow/Orange - warm
      color = M5Cardputer.Display.color565(depthBrightness, depthBrightness/2, 0);
    }

    // Draw warp streaks at high speed (stars stretch into lines!)
    if (!stars[i].isStatic && starfieldSpeed > 2.5) { // Start streaks earlier
      // Calculate previous position for streak
      int oldScreenX = (int)((stars[i].x / oldZ) * 100) + 120;
      int oldScreenY = (int)((stars[i].y / oldZ) * 100) + 67;

      // Draw streak line from old position to new position
      if (oldScreenX >= 0 && oldScreenX < 240 && oldScreenY >= 0 && oldScreenY < 135) {
        // Fade the streak color for effect
        uint16_t streakColor;
        if (stars[i].baseColor == 0) {
          streakColor = M5Cardputer.Display.color565(depthBrightness/2, depthBrightness/2, depthBrightness/2);
        } else if (stars[i].baseColor == 1) {
          streakColor = M5Cardputer.Display.color565(depthBrightness/8, depthBrightness/4, depthBrightness/2);
        } else if (stars[i].baseColor == 2) {
          streakColor = M5Cardputer.Display.color565(0, depthBrightness/2, depthBrightness/2);
        } else if (stars[i].baseColor == 3) {
          streakColor = M5Cardputer.Display.color565(0, depthBrightness/2, depthBrightness/6);
        } else if (stars[i].baseColor == 4) {
          streakColor = M5Cardputer.Display.color565(depthBrightness/2, depthBrightness/8, depthBrightness/2);
        } else {
          streakColor = M5Cardputer.Display.color565(depthBrightness/2, depthBrightness/4, 0);
        }
        M5Cardputer.Display.drawLine(oldScreenX, oldScreenY, screenX, screenY, streakColor);
      }
    }

    // Draw star based on type and size
    if (starSize == 1 || stars[i].type == 0) {
      // Simple dot
      M5Cardputer.Display.drawPixel(screenX, screenY, color);
    } else if (stars[i].type == 1 || starSize == 2) {
      // Small cross
      M5Cardputer.Display.fillRect(screenX-1, screenY-1, 3, 3, color);
    } else {
      // Large cross (close stars)
      M5Cardputer.Display.drawPixel(screenX, screenY, color);
      M5Cardputer.Display.drawPixel(screenX-1, screenY, color);
      M5Cardputer.Display.drawPixel(screenX+1, screenY, color);
      M5Cardputer.Display.drawPixel(screenX, screenY-1, color);
      M5Cardputer.Display.drawPixel(screenX, screenY+1, color);
      M5Cardputer.Display.drawPixel(screenX-2, screenY, color);
      M5Cardputer.Display.drawPixel(screenX+2, screenY, color);
      M5Cardputer.Display.drawPixel(screenX, screenY-2, color);
      M5Cardputer.Display.drawPixel(screenX, screenY+2, color);
    }
  }

  // Draw status text in bottom RIGHT corner if provided
  if (statusText.length() > 0) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_YELLOW);
    int textWidth = statusText.length() * 6; // Approximate width
    M5Cardputer.Display.drawString(statusText, 235 - textWidth, 125);
  }
}

void setup() {
  Serial.begin(115200);
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);

  // Try to enable Unicode/emoji rendering
  M5Cardputer.Display.setFont(&fonts::Font0); // Default font
  M5Cardputer.Display.setTextWrap(false);
  M5Cardputer.Display.cp437(true); // Enable extended ASCII

  // Initialize WiFi mode early but don't connect yet
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize Speaker
  M5Cardputer.Speaker.begin();
  M5Cardputer.Speaker.setVolume(80); // Lower volume (0-255)

  // Load settings first
  loadSettings();

  // Load saved networks from Preferences BEFORE boot animation
  // This prevents Preferences access from interfering with SD card during animation
  preferences.begin("wifi", false);
  numSavedNetworks = preferences.getInt("count", 0);
  Serial.println("=== SAVED NETWORKS ===");
  Serial.println("Count: " + String(numSavedNetworks));
  for (int i = 0; i < numSavedNetworks && i < MAX_SAVED_NETWORKS; i++) {
    savedSSIDs[i] = preferences.getString(("ssid" + String(i)).c_str(), "");
    savedPasswords[i] = preferences.getString(("pass" + String(i)).c_str(), "");
    Serial.println("  [" + String(i) + "] SSID: '" + savedSSIDs[i] + "' (len=" + String(savedSSIDs[i].length()) + ")");
  }
  Serial.println("======================");
  preferences.end();

  // Initialize background services
  initBackgroundServices();

  // Auto-connect WiFi BEFORE boot animation (if we have saved networks)
  if (numSavedNetworks > 0) {
    Serial.println("=== AUTO-CONNECT: Scanning for saved networks ===");

    // Initialize starfield
    initStarfield();

    // Quick scan to see which networks are available
    WiFi.disconnect();
    delay(100);

    // Start the scan and show animation
    WiFi.scanNetworks(true); // Async scan

    // Animate starfield while scanning - yellow text lower right
    while (WiFi.scanComplete() < 0) {
      drawStarfield("Scanning...");
      delay(30); // Faster animation
    }

    int n = WiFi.scanComplete();
    Serial.println(">>> Found " + String(n) + " networks");

    // Check which of our saved networks are available
    String bestNetwork = "";
    String bestPassword = "";
    int bestRSSI = -1000;

    for (int i = 0; i < numSavedNetworks; i++) {
      if (savedSSIDs[i].length() > 0) {
        // Check if this saved network is in the scan results
        for (int j = 0; j < n; j++) {
          if (WiFi.SSID(j) == savedSSIDs[i]) {
            int rssi = WiFi.RSSI(j);
            Serial.println(">>> Found saved network: " + savedSSIDs[i] + " (RSSI: " + String(rssi) + ")");
            // Pick the strongest available saved network
            if (rssi > bestRSSI) {
              bestRSSI = rssi;
              bestNetwork = savedSSIDs[i];
              bestPassword = savedPasswords[i];
            }
          }
        }
      }
    }

    // Connect to the best available network
    if (bestNetwork.length() > 0) {
      Serial.println(">>> Auto-connecting to: " + bestNetwork + " (RSSI: " + String(bestRSSI) + ")");

      // Show connecting message on starfield with network name
      WiFi.begin(bestNetwork.c_str(), bestPassword.c_str());

      // Shorter connection animation - show network name
      String connectMsg = "Connecting to " + bestNetwork;
      for (int i = 0; i < 20; i++) {
        drawStarfield(connectMsg);
        delay(30);
      }
    } else {
      Serial.println(">>> No saved networks found in range");
      drawStarfield("No networks found");
      delay(300);
    }

    WiFi.scanDelete(); // Clean up scan results

    // Keep starfield screen visible briefly before transition
    drawStarfield("");  // Draw one more frame without text
    delay(50);

    starsInitialized = false; // Reset for next boot
  }

  // Play boot animation
  playBootAnimation();

  // Initialize and show star rain landing page immediately
  initStarRain(STARRAIN_LANDING);

  // Animate several frames immediately to create smooth transition into loop
  for (int i = 0; i < 15; i++) {
    updateStarRain();
    drawStarRain();
    delay(10);
  }

  currentState = STAR_LANDING_PAGE;

  // Mark when boot completes - defer expensive WiFi ops for smooth animation
  bootCompleteTime = millis();
}

void handleBackgroundWiFi() {
  // Skip expensive operations during boot animation for smooth star rain
  if (skipWiFiOpsUntilReady && (millis() - bootCompleteTime < 2000)) {
    return; // Skip all WiFi operations for first 2 seconds after boot
  }
  skipWiFiOpsUntilReady = false; // After delay, run normally

  // Check WiFi status without blocking - resync time periodically when connected
  if (WiFi.status() == WL_CONNECTED) {
    // Update global WiFi status if not already set
    if (!wifiConnected) {
      wifiConnected = true;
      wifiSSID = WiFi.SSID();
      Serial.println("WiFi connected! IP: " + WiFi.localIP().toString());
    }

    // Auto-detect timezone on first connection if enabled
    static bool timezoneDetected = false;
    if (settings.timezoneAuto && !timezoneDetected) {
      autoDetectTimezone();
      timezoneDetected = true;
    }

    // Sync time if never synced, or every hour
    unsigned long timeSinceLastSync = millis() - lastSyncTime;
    bool shouldSync = !timeIsSynced || (timeSinceLastSync > 3600000); // 1 hour in milliseconds

    if (shouldSync) {
      // Sync time using timezone with DST support
      applyTimezone();
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        timeIsSynced = true;
        lastSyncTime = millis();

        // Save synced time to preferences
        time_t now = time(nullptr);
        settings.lastKnownTime = now;
        preferences.begin("settings", false);
        preferences.putULong("lastTime", (unsigned long)now);
        preferences.end();
      }
    }
  } else {
    // Update global WiFi status if disconnected
    if (wifiConnected) {
      wifiConnected = false;
      wifiSSID = "Not Connected";
      Serial.println("WiFi disconnected");
    }
  }
}

// Helper function to update audio - call this frequently to prevent skipping
void updateAudioIfPlaying() {
  if (isRadioPlaying()) {
    updateRadioPlayback();  // Update audio manager's radio stream
    updateRadioApp();       // Update radio app's UI state
  } else if (isAudioPlaying()) {
    updateMusicPlayback();  // Update audio manager's music playback
  }
}

void loop() {
  // Update audio at the very start of the loop to minimize gaps
  updateAudioIfPlaying();

  // Handle star rain landing page FIRST - skip expensive M5Cardputer.update() for smooth animation
  if (currentState == STAR_LANDING_PAGE) {
    // Skip WiFi operations during initial animation
    if (!skipWiFiOpsUntilReady) {
      handleBackgroundWiFi();
    }

    updateStarRain();
    drawStarRain();

    // Lightweight keyboard check without full M5Cardputer.update()
    M5Cardputer.Keyboard.updateKeyList();
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
      stopStarRain();
      if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 100);

      currentState = MAIN_MENU;
      lastActivityTime = millis(); // Reset activity timer
      drawScreen();
    }

    delay(10);
    return;
  }

  // Normal operation for all other states
  M5Cardputer.update();

  // Handle background WiFi connection (non-blocking)
  handleBackgroundWiFi();

  // Handle screensaver
  if (screensaverActive) {
    updateStarRain();
    drawStarRain();

    // Update audio during screensaver to prevent skipping
    updateAudioIfPlaying();

    // Check if any key is pressed to exit screensaver
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
      stopStarRain();
      screensaverActive = false;
      lastActivityTime = millis();

      // Redraw screen based on current state
      if (currentState == MAIN_MENU) {
        drawScreen(false);
      } else if (currentState == APPS_MENU) {
        drawScreen(true);
      } else if (currentState == WIFI_SCAN) {
        drawWiFiScan();
      } else if (currentState == WIFI_PASSWORD) {
        drawWiFiPassword();
      } else if (currentState == WIFI_SAVED) {
        drawWiFiSaved();
      } else if (currentState == SCREEN_VIEW) {
        // Redraw based on which app/screen is active
        if (currentScreenNumber == 1) {
          // File Manager - redraw based on fmState
          if (fmState == FM_FOLDER_VIEW) drawFolderView();
          else if (fmState == FM_FILE_VIEWER) drawFileViewer();
        } else if (currentScreenNumber == 2) {
          // WiFi Fun - redraw based on wifiFunState
          if (wifiFunState == WIFI_FUN_MENU) drawWiFiFunMenu();
          else if (wifiFunState == FAKE_WIFI_INPUT) drawFakeWiFiInput();
          else if (wifiFunState == FAKE_WIFI_RUNNING) drawFakeWiFiRunning();
          else if (wifiFunState == PORTALS_MENU) drawPortalsMenu();
          else if (wifiFunState == PORTAL_INPUT) drawPortalInput();
          else if (wifiFunState == BIG_PARTY_MENU) drawBigPartyMenu();
          else if (wifiFunState == PARTY_TIME_EDIT) drawPartyTimeEdit();
          else if (wifiFunState == PARTY_TIME_RUNNING) drawPartyTimeRunning();
          else if (wifiFunState == RICKROLL_RUNNING) drawRickRoll();
          else if (wifiFunState == TV_B_GONE_RUNNING) drawTVBGone();
          else if (wifiFunState == ANALYTICS_MENU) drawAnalyticsMenu();
          else if (wifiFunState == SPEEDOMETER_RUNNING) drawSpeedometer();
          else if (wifiFunState == PROBE_SNIFFER_RUNNING) drawProbeSniffer();
          else if (wifiFunState == PORTAL_GAMES_RUNNING) drawPortalGames();
          else if (wifiFunState == PORTAL_RUNNING_STATE) drawPortalScreen();
        } else if (currentScreenNumber == 3) {
          // Transfer Portal - redraw based on transferState
          if (transferState == TRANSFER_MENU || transferState == TRANSFER_STOPPED) {
            drawTransferMenu();
          } else if (transferState == TRANSFER_RUNNING) {
            drawTransferRunning();
          }
        } else if (currentScreenNumber == 4) {
          drawRadioUI();
        } else if (currentScreenNumber == 5) {
          drawTerminal();
        } else if (currentScreenNumber == 6) {
          drawMusicPlayer();
        } else if (currentScreenNumber == 7) {
          // The Book - redraw based on bookState
          if (bookState == BOOK_SEARCH) drawBookSearch();
          else if (bookState == BOOK_TOC) drawTableOfContents();
          else if (bookState == BOOK_RESULTS) drawBookResults();
          else if (bookState == BOOK_ARTICLE) drawBookArticle();
        } else if (currentScreenNumber == 8) {
          // Games placeholder
          M5Cardputer.Display.fillScreen(TFT_BLACK);
          drawStatusBar(false);
          M5Cardputer.Display.setTextSize(2);
          M5Cardputer.Display.setTextColor(TFT_PURPLE);
          M5Cardputer.Display.drawString("Games", 85, 30);
          M5Cardputer.Display.setTextSize(1);
          M5Cardputer.Display.setTextColor(TFT_WHITE);
          M5Cardputer.Display.drawString("Coming Soon!", 75, 60);
          M5Cardputer.Display.setTextColor(TFT_DARKGREY);
          M5Cardputer.Display.drawString("Snake, Tetris, and more...", 35, 85);
        } else if (currentScreenNumber == 9) {
          drawRoadmap();
        } else if (currentScreenNumber == 10) {
          drawWiFiScan();
        } else if (currentScreenNumber == 11) {
          drawSettingsMenu();
        } else if (currentScreenNumber == 12) {
          drawMusicMenu();
        } else if (currentScreenNumber == 13) {
          // Music Tools - redraw based on state
          if (musicToolsState == TOOLS_MENU) {
            drawMusicToolsMenu();
          } else if (musicToolsState == GUITAR_TUNER) {
            drawGuitarTuner();
          } else if (musicToolsState == AUDIO_VISUALIZER) {
            drawAudioVisualizer();
          }
        }
      }

      return;
    }

    // Use shorter delay when audio is playing to keep buffer full
    if (isRadioPlaying() || isAudioPlaying()) {
      delay(1);  // Minimal delay for smooth audio
    } else {
      delay(10);  // Normal delay when no audio
    }
    return;
  }

  // Check for inactivity timeout (works on all screens)
  if (!screensaverActive &&
      millis() - lastActivityTime > SCREENSAVER_TIMEOUT) {
    // Start screensaver
    initStarRain(STARRAIN_SCREENSAVER);
    screensaverActive = true;
    delay(10);
    return;
  }

  // Visual debug removed - no longer needed

  // Auto-connect WiFi after delay (only on main menu, not in WiFi settings)
  if (!autoConnectAttempted && autoConnectTriggerTime > 0 && millis() >= autoConnectTriggerTime &&
      currentState == MAIN_MENU && !wifiConnected) {
    autoConnectAttempted = true;
    autoConnectTriggerTime = 0;

    Serial.println("=== AUTO-CONNECT: Scanning for saved networks ===");

    // Show scanning message
    M5Cardputer.Display.fillRect(0, 125, 240, 10, TFT_BLUE);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString("Scanning...", 80, 125);

    // Quick scan to see which networks are available
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();

    Serial.println(">>> Found " + String(n) + " networks");

    // Check which of our saved networks are available
    String bestNetwork = "";
    String bestPassword = "";
    int bestRSSI = -1000;

    for (int i = 0; i < numSavedNetworks; i++) {
      if (savedSSIDs[i].length() > 0) {
        // Check if this saved network is in the scan results
        for (int j = 0; j < n; j++) {
          if (WiFi.SSID(j) == savedSSIDs[i]) {
            int rssi = WiFi.RSSI(j);
            Serial.println(">>> Found saved network: " + savedSSIDs[i] + " (RSSI: " + String(rssi) + ")");
            // Pick the strongest available saved network
            if (rssi > bestRSSI) {
              bestRSSI = rssi;
              bestNetwork = savedSSIDs[i];
              bestPassword = savedPasswords[i];
            }
          }
        }
      }
    }

    // Connect to the best available network
    if (bestNetwork.length() > 0) {
      Serial.println(">>> Auto-connecting to: " + bestNetwork + " (RSSI: " + String(bestRSSI) + ")");

      M5Cardputer.Display.fillRect(0, 125, 240, 10, TFT_GREEN);
      M5Cardputer.Display.setTextColor(TFT_WHITE);
      M5Cardputer.Display.drawString("Connecting: " + bestNetwork, 5, 125);

      WiFi.begin(bestNetwork.c_str(), bestPassword.c_str());
      delay(1000); // Show message for 1 second
    } else {
      Serial.println(">>> No saved networks found in range");

      M5Cardputer.Display.fillRect(0, 125, 240, 10, TFT_ORANGE);
      M5Cardputer.Display.setTextColor(TFT_BLACK);
      M5Cardputer.Display.drawString("No saved networks nearby", 20, 125);
      delay(1500);
    }

    drawScreen(false); // Redraw to clear message
  }

  // Handle terminal star rain command
  if (terminalStarRainRequested && currentState == SCREEN_VIEW && currentScreenNumber == 5) {
    terminalStarRainRequested = false;
    terminalStarRainActive = true;
    initStarRain(STARRAIN_TERMINAL);
    delay(10);
    return;
  }

  // Handle terminal star rain animation
  if (terminalStarRainActive) {
    updateStarRain();
    drawStarRain();

    // Check if backtick is pressed to exit
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
      for (auto key : status.word) {
        if (key == '`') {
          stopStarRain();
          terminalStarRainActive = false;
          drawTerminal();
          delay(10);
          return;
        }
      }
    }

    delay(10);
    return;
  }

  // Update audio before keyboard handling to prevent skipping during input
  updateAudioIfPlaying();

  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
      // Reset activity timer on any key press
      lastActivityTime = millis();

      // Temporarily mute audio during navigation to avoid hearing skips
      if (isRadioPlaying() || isAudioPlaying()) {
        lastKeyPressTime = millis();
        if (!audioCurrentlyMuted) {
          temporarilyMuteAudio();
          audioCurrentlyMuted = true;
        }
      }

      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

      // Handle File Manager input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 1) {
        if (!sdCardMounted) {
          // SD card error screen, allow back button
          for (auto key : status.word) {
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              currentState = APPS_MENU;
              drawScreen(true);
              return;
            }
          }
          return;
        }
        
        if (fmState == FM_FOLDER_VIEW) {
          if (status.enter) {
            if (fileCount > 0) {
              if (fileInfoList[selectedFileIndex].type == TYPE_FOLDER) {
                // Navigate into folder - build full path
                if (currentPath.endsWith("/")) {
                  currentPath = currentPath + fileInfoList[selectedFileIndex].name;
                } else {
                  currentPath = currentPath + "/" + fileInfoList[selectedFileIndex].name;
                }
                selectedFileIndex = 0;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
                loadFolder(currentPath);
              } else {
                // Open file - build full path
                String filePath;
                if (currentPath.endsWith("/")) {
                  filePath = currentPath + fileInfoList[selectedFileIndex].name;
                } else {
                  filePath = currentPath + "/" + fileInfoList[selectedFileIndex].name;
                }
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
                loadFile(filePath);
              }
            }
          }

          if (status.del) {
            // Delete key pressed
            extern int selectedCount;
            if (selectedCount > 0) {
              // Batch delete selected files
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
              showBatchDeleteConfirmation();
            } else if (fileCount > 0) {
              // Single file delete
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
              showDeleteConfirmation();
            }
            return;
          }

          // Handle search mode first
          extern String searchQuery;
          extern bool searchActive;

          if (status.del && searchActive) {
            // Backspace in search
            if (searchQuery.length() > 0) {
              searchQuery.remove(searchQuery.length() - 1);
              selectedFileIndex = 0;
              drawFolderView();
            }
            return;
          }

          for (auto key : status.word) {
            if (key == '/' && !searchActive) {
              // Enter search mode
              searchActive = true;
              searchQuery = "";
              selectedFileIndex = 0;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
              drawFolderView();
              return;
            } else if (key == '`' && searchActive) {
              // Exit search mode
              searchActive = false;
              searchQuery = "";
              selectedFileIndex = 0;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 50);
              drawFolderView();
              return;
            } else if (searchActive && key >= 32 && key <= 126) {
              // Add character to search
              searchQuery += (char)key;
              selectedFileIndex = 0;
              drawFolderView();
              return;
            }

            // Space bar to toggle selection
            if (key == ' ') {
              if (fileCount > 0 && !searchActive) {
                extern bool fileSelected[50];
                extern int selectedCount;
                // Toggle selection for current file
                fileSelected[selectedFileIndex] = !fileSelected[selectedFileIndex];
                selectedCount += fileSelected[selectedFileIndex] ? 1 : -1;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 50);
                drawFolderView();
              }
              return;
            }

            if (key == 'n' || key == 'N') {
              // Create new folder
              if (searchActive) continue;  // Skip if searching
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(700, 50);
              createFolder();
              return;
            } else if (key == 'r' || key == 'R') {
              // Rename file
              if (searchActive) continue;  // Skip if searching
              if (fileCount > 0) {
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
                renameFile();
              }
              return;
            } else if (key == 'x' || key == 'X') {
              // Cut file
              if (searchActive) continue;  // Skip if searching
              if (fileCount > 0) {
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(900, 50);
                cutFile();
              }
              return;
            } else if (key == 'c' || key == 'C') {
              // Copy file
              if (searchActive) continue;  // Skip if searching
              if (fileCount > 0) {
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
                copyFile();
              }
              return;
            } else if (key == 'v' || key == 'V') {
              // Paste file
              if (searchActive) continue;  // Skip if searching
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1100, 50);
              pasteFile();
              return;
            }

            if (!searchActive) {
              handleFileManagerNavigation(key);
            }
            if (key == '`') {
              // Go back to parent directory or exit
              if (currentPath == "/") {
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
                currentState = APPS_MENU;
                drawScreen(true);
              } else {
                // Go to parent directory
                int lastSlash = currentPath.lastIndexOf('/', currentPath.length() - 2);
                currentPath = currentPath.substring(0, lastSlash + 1);
                selectedFileIndex = 0;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
                loadFolder(currentPath);
              }
              return;
            }
          }
          return;
        } else if (fmState == FM_FILE_VIEWER) {
          // File viewer - handle navigation and exit
          if (status.enter) {
            // Toggle play/pause for audio files
            if (isAudioPlaying()) {
              stopAudioPlayback();
            } else if (currentAudioPath != "") {
              playAudioFile(currentAudioPath);
            }
          }

          for (auto key : status.word) {
            handleFileManagerNavigation(key); // Allow scrolling through files
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              stopGifPlayback(); // Stop GIF if playing
              stopAudioPlayback(); // Stop audio if playing
              fmState = FM_FOLDER_VIEW;
              drawFolderView();
              return;
            }
          }
          return;
        }
      }
      
      // Handle File Transfer app input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 3) {
        if (transferState == TRANSFER_MENU) {
          if (status.enter) {
            if (WiFi.status() == WL_CONNECTED) {
              startBackgroundTransfer();
            }
          }

          for (auto key : status.word) {
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              stopWebServer();  // CRITICAL: Clean up WebServer to free RAM!
              currentState = APPS_MENU;
              drawScreen(true);
              return;
            }
          }
          return;
        } else if (transferState == TRANSFER_RUNNING) {
          // Allow stopping the server when it's running
          for (auto key : status.word) {
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              stopWebServer();  // CRITICAL: Clean up WebServer to free RAM!
              currentState = APPS_MENU;
              drawScreen(true);
              return;
            }
          }
          if (status.del) {
            // ESC stops the server and returns to menu
            stopWebServer();
            return;
          }
          return;
        }
      }
      
      // Handle WiFi Fun app input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 2) {
        if (wifiFunState == WIFI_FUN_MENU) {
          if (status.enter) {
            if (wifiFunMenuIndex == 0) {
              // Fake WiFi selected
              wifiFunState = FAKE_WIFI_INPUT;
              fakeSSID = "";
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
              drawFakeWiFiInput();
            } else if (wifiFunMenuIndex == 1) {
              // Portals submenu selected
              wifiFunState = PORTALS_MENU;
              portalsMenuIndex = 0;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
              drawPortalsMenu();
            } else if (wifiFunMenuIndex == 2) {
              // Wi-Fi Party submenu selected
              wifiFunState = BIG_PARTY_MENU;
              bigPartyMenuIndex = 0;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
              drawBigPartyMenu();
            } else if (wifiFunMenuIndex == 3) {
              // TURN THIS TV OFF selected
              wifiFunState = TV_B_GONE_RUNNING;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
              startTVBGone();
            } else if (wifiFunMenuIndex == 4) {
              // Analytics submenu selected
              wifiFunState = ANALYTICS_MENU;
              analyticsMenuIndex = 0;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
              drawAnalyticsMenu();
            }
          }

          for (auto key : status.word) {
            handleWiFiFunNavigation(key);
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              currentState = APPS_MENU;
              drawScreen(true);
              return;
            }
          }
          return;
        } else if (wifiFunState == FAKE_WIFI_INPUT) {
          // Handle SSID input
          for (auto key : status.word) {
            if (key >= 32 && key <= 126) {
              fakeSSID += (char)key;
              drawFakeWiFiInput();
            }
          }
          
          if (status.del) {
            if (fakeSSID.length() > 0) {
              fakeSSID.remove(fakeSSID.length() - 1);
              drawFakeWiFiInput();
            }
          }
          
          if (status.enter) {
            // Start fake WiFi in background and return to menu
            startBackgroundFakeAP(fakeSSID);
            wifiFunState = WIFI_FUN_MENU;
            drawWiFiFunMenu();
          }

          for (auto key : status.word) {
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              wifiFunState = WIFI_FUN_MENU;
              drawWiFiFunMenu();
              return;
            }
          }
          return;
        } else if (wifiFunState == PORTAL_INPUT) {
          // Handle portal SSID input
          for (auto key : status.word) {
            if (key >= 32 && key <= 126) {
              portalSSIDInput += (char)key;
              drawPortalInput();
            }
          }

          if (status.del) {
            if (portalSSIDInput.length() > 0) {
              portalSSIDInput.remove(portalSSIDInput.length() - 1);
              drawPortalInput();
            }
          }

          if (status.enter) {
            // Start portal in background and return to menu
            startBackgroundPortal(portalSSIDInput);
            wifiFunState = WIFI_FUN_MENU;
            drawWiFiFunMenu();
          }

          for (auto key : status.word) {
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              wifiFunState = WIFI_FUN_MENU;
              drawWiFiFunMenu();
              return;
            }
          }
          return;
        } else if (wifiFunState == PORTALS_MENU && pmState == PM_LIST) {
          if (status.enter) {
            if (portalsMenuIndex == 0) {
              // portalDECK - Enter portal manager (pmState will change)
              enterPortalManager();
              return;
            } else if (portalsMenuIndex == 1) {
              // portalGAMES - Start multiplayer word game
              startPortalGames();
            }
          }

          for (auto key : status.word) {
            handleWiFiFunNavigation(key);
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              wifiFunState = WIFI_FUN_MENU;
              drawWiFiFunMenu();
              return;
            }
          }
          return;
        } else if (wifiFunState == BIG_PARTY_MENU) {
          if (status.enter) {
            if (bigPartyMenuIndex == 0) {
              // Rick Roll selected
              wifiFunState = RICKROLL_RUNNING;
              startRickRoll();
              drawRickRoll();
            } else if (bigPartyMenuIndex == 1) {
              // Party Time selected - go to edit screen
              wifiFunState = PARTY_TIME_EDIT;
              partyTimeInput = "";
              partyTimeListScroll = 0;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
              drawPartyTimeEdit();
            }
          }

          for (auto key : status.word) {
            handleWiFiFunNavigation(key);
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              wifiFunState = WIFI_FUN_MENU;
              drawWiFiFunMenu();
              return;
            }
          }
          return;
        } else if (wifiFunState == RICKROLL_RUNNING) {
          // Any key stops Rick Roll
          for (auto key : status.word) {
            stopGifPlayback();
            stopRickRoll();
            wifiFunState = BIG_PARTY_MENU;
            drawBigPartyMenu();
            return;
          }
          if (status.enter || status.del) {
            stopGifPlayback();
            stopRickRoll();
            wifiFunState = BIG_PARTY_MENU;
            drawBigPartyMenu();
          }
          return;
        } else if (wifiFunState == TV_B_GONE_RUNNING) {
          // ESC or ` key stops TV-B-Gone
          if (status.del) {
            stopTVBGone();
            return;
          }
          for (auto key : status.word) {
            if (key == '`') {
              stopTVBGone();
              wifiFunState = WIFI_FUN_MENU;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              drawWiFiFunMenu();
              return;
            }
          }
          return;
        } else if (wifiFunState == ANALYTICS_MENU) {
          if (status.enter) {
            if (analyticsMenuIndex == 0) {
              // Speedometer selected
              wifiFunState = SPEEDOMETER_RUNNING;
              startSpeedometer();
            } else if (analyticsMenuIndex == 1) {
              // Probe Sniffer selected
              wifiFunState = PROBE_SNIFFER_RUNNING;
              startProbeSniffer();
            }
          }

          for (auto key : status.word) {
            handleWiFiFunNavigation(key);
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              wifiFunState = WIFI_FUN_MENU;
              drawWiFiFunMenu();
              return;
            }
          }
          return;
        } else if (wifiFunState == SPEEDOMETER_RUNNING) {
          // Spacebar starts test, backtick stops
          for (auto key : status.word) {
            if (key == ' ') {
              // Start speed test on spacebar
              extern bool speedTestRunning;
              extern bool speedTestComplete;
              extern unsigned long speedTestStartTime;
              extern float uploadSpeed;
              extern float downloadSpeed;

              if (!speedTestRunning) {
                speedTestRunning = true;
                speedTestComplete = false;
                speedTestStartTime = millis();
                uploadSpeed = 0.0;
                downloadSpeed = 0.0;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
                drawSpeedometer();
              }
              return;
            } else if (key == '`') {
              stopSpeedometer();
              wifiFunState = ANALYTICS_MENU;
              drawAnalyticsMenu();
              return;
            }
          }
          return;
        } else if (wifiFunState == PROBE_SNIFFER_RUNNING) {
          // Handle scrolling and exit
          for (auto key : status.word) {
            if (key == ';' || key == ',') {
              // Scroll up (show older requests)
              extern int probeRequestScrollPos;
              extern int probeRequestCount;
              if (probeRequestScrollPos < probeRequestCount - 6) {
                probeRequestScrollPos++;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
                drawProbeSniffer();
              }
            } else if (key == '.' || key == '/') {
              // Scroll down (show newer requests)
              extern int probeRequestScrollPos;
              if (probeRequestScrollPos > 0) {
                probeRequestScrollPos--;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
                drawProbeSniffer();
              }
            } else if (key == '`') {
              stopProbeSniffer();
              return;
            }
          }
          return;
        } else if (wifiFunState == PORTAL_GAMES_RUNNING) {
          // Any key stops Portal Games
          for (auto key : status.word) {
            if (key == '`') {
              stopPortalGames();
              return;
            }
          }
          return;
        } else if (wifiFunState == PARTY_TIME_EDIT) {
          // Handle Party Time SSID input
          for (auto key : status.word) {
            if (key >= 32 && key <= 126 && key != '+' && key != '`') {
              partyTimeInput += (char)key;
              drawPartyTimeEdit();
            } else if (key == '+') {
              // Add SSID to list
              if (apListCount < 10 && partyTimeInput.length() > 0) {
                apList[apListCount] = partyTimeInput;
                apListCount++;
                partyTimeInput = "";
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 50);
                // Auto-scroll to show new item
                if (apListCount > 4) {
                  partyTimeListScroll = apListCount - 4;
                }
                drawPartyTimeEdit();
              }
            }
          }

          if (status.del) {
            if (partyTimeInput.length() > 0) {
              partyTimeInput.remove(partyTimeInput.length() - 1);
              drawPartyTimeEdit();
            }
          }

          if (status.enter) {
            // Start Party Time broadcast
            startPartyTime();
          }

          for (auto key : status.word) {
            handleWiFiFunNavigation(key);
            if (key == '`') {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              wifiFunState = BIG_PARTY_MENU;
              drawBigPartyMenu();
              return;
            }
          }
          return;
        } else if (wifiFunState == PARTY_TIME_RUNNING) {
          // Handle a/s/d for save/load/manage
          for (auto key : status.word) {
            if (key == 'a' || key == 'A') {
              // Switch to save slot selection state
              wifiFunState = PARTY_TIME_SAVE_SLOT;
              showPartyTimeSaveMenu();
              return;
            } else if (key == 's' || key == 'S') {
              // Switch to load slot selection state
              wifiFunState = PARTY_TIME_LOAD_SLOT;
              showPartyTimeLoadMenu();
              return;
            } else if (key == 'd' || key == 'D') {
              // Show manage menu (informational only, any key closes it)
              showPartyTimeManageMenu();
              delay(2000); // Show for 2 seconds
              drawPartyTimeRunning();
              return;
            } else if (key == '`') {
              stopPartyTime();
              return;
            }
          }
          return;
        } else if (wifiFunState == PARTY_TIME_SAVE_SLOT) {
          // Waiting for slot number to save
          for (auto key : status.word) {
            if (key >= '1' && key <= '5') {
              savePartyTimeSet(key - '0');
              wifiFunState = PARTY_TIME_RUNNING;
              drawPartyTimeRunning();
              return;
            } else {
              // Any other key cancels
              wifiFunState = PARTY_TIME_RUNNING;
              drawPartyTimeRunning();
              return;
            }
          }
          return;
        } else if (wifiFunState == PARTY_TIME_LOAD_SLOT) {
          // Waiting for slot number to load
          for (auto key : status.word) {
            if (key >= '1' && key <= '5') {
              loadPartyTimeSet(key - '0');
              wifiFunState = PARTY_TIME_RUNNING;
              drawPartyTimeRunning();
              return;
            } else {
              // Any other key cancels
              wifiFunState = PARTY_TIME_RUNNING;
              drawPartyTimeRunning();
              return;
            }
          }
          return;
        }
      }

      // Handle Portal Manager input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == PORTAL_MANAGER_ACTIVE && pmState == PM_LIST) {
        // PM_LIST state - handle navigation and actions
        if (status.enter) {
          if (numSavedPortals > 0) {
            // Launch selected portal
            startCaptivePortalFromProfile(savedPortals[selectedPortalIndex]);
            return;
          } else {
            // No portals - start creating one
            pmState = PM_CREATE_NAME;
            portalInputBuffer = "";
            currentPortal.name = "";
            currentPortal.ssid = "";
            currentPortal.htmlPath = "";
            currentPortal.useBuiltIn = true;
            drawPortalCreate();
            return;
          }
        }

        if (status.del) {
          if (numSavedPortals > 0) {
            deletePortal(selectedPortalIndex);
            if (selectedPortalIndex >= numSavedPortals && selectedPortalIndex > 0) {
              selectedPortalIndex--;
            }
            drawPortalList();
          }
        }

        for (auto key : status.word) {
          if (key == 'n') {
            // Start creating new portal
            pmState = PM_CREATE_NAME;
            portalInputBuffer = "";
            currentPortal.name = "";
            currentPortal.ssid = "";
            currentPortal.htmlPath = "";
            currentPortal.useBuiltIn = true;
            drawPortalCreate();
            return;
          } else if (key == 'd') {
            // Quick demo - launch built-in portal immediately
            PortalProfile demoPortal;
            demoPortal.name = "Demo";
            demoPortal.ssid = "Laboratory Demo";
            demoPortal.htmlPath = "";
            demoPortal.useBuiltIn = true;
            startCaptivePortalFromProfile(demoPortal);
            return;
          } else if (key == 'u') {
            // Show upload help
            pmState = PM_UPLOAD_HELP;
            drawPortalUploadHelp();
            return;
          } else if (key == '`') {
            // Back to portals menu
            wifiFunState = PORTALS_MENU;
            drawPortalsMenu();
            return;
          }

          handlePortalManagerNavigation(key);
        }
        return;
      } else if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == PORTAL_MANAGER_ACTIVE && pmState != PM_LIST) {
        // Portal Manager is active (other states)
        if (pmState == PM_CREATE_NAME || pmState == PM_CREATE_SSID || pmState == PM_CREATE_FILE) {
          // Handle text input
          for (auto key : status.word) {
            if (key >= 32 && key <= 126) {
              portalInputBuffer += (char)key;
              drawPortalCreate();
            }
          }

          if (status.del) {
            if (portalInputBuffer.length() > 0) {
              portalInputBuffer.remove(portalInputBuffer.length() - 1);
              drawPortalCreate();
            }
          }

          if (status.enter) {
            if (pmState == PM_CREATE_NAME) {
              // Move to SSID input
              currentPortal.name = portalInputBuffer;
              portalInputBuffer = "";
              pmState = PM_CREATE_SSID;
              drawPortalCreate();
            } else if (pmState == PM_CREATE_SSID) {
              // Move to file path input
              currentPortal.ssid = portalInputBuffer;
              portalInputBuffer = "";
              pmState = PM_CREATE_FILE;
              drawPortalCreate();
            } else if (pmState == PM_CREATE_FILE) {
              // Save portal
              currentPortal.htmlPath = portalInputBuffer;
              currentPortal.useBuiltIn = (portalInputBuffer.length() == 0);
              savePortal(currentPortal);
              pmState = PM_LIST;
              enterPortalManager();
            }
          }

          for (auto key : status.word) {
            if (key == '`') {
              // Cancel creation
              pmState = PM_LIST;
              enterPortalManager();
              return;
            }
          }
          return;
        } else if (pmState == PM_UPLOAD_HELP) {
          for (auto key : status.word) {
            if (key == '`') {
              pmState = PM_LIST;
              drawPortalList();
              return;
            }
          }
          return;
        }
      }

      // Handle Radio app input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 4) {
        for (auto key : status.word) {
          handleRadioNavigation(key);
          if (key == '`') {
            // Back to music menu
            exitRadioApp();
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
            currentScreenNumber = 12;
            musicMenuIndex = 1;  // Set to Radio position
            drawMusicMenu();
            return;
          }
        }
        return;
      }

      // Handle Terminal app input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 5) {
        // Terminal is always active when screen 5 is displayed
        // Check for Fn modifier for scrolling and history
        bool fnPressed = status.fn;

        for (auto key : status.word) {
          if (fnPressed && (key == ';' || key == ':')) {
            // Fn+; (up arrow) - Previous command (scroll up in history)
            extern int historyIndex;
            extern std::vector<String> commandHistory;
            extern String currentInput;
            if (commandHistory.size() > 0) {
              if (historyIndex < 0) {
                historyIndex = commandHistory.size() - 1;
              } else if (historyIndex > 0) {
                historyIndex--;
              }
              if (historyIndex >= 0 && historyIndex < commandHistory.size()) {
                currentInput = commandHistory[historyIndex];
                drawTerminal();
              }
            }
          } else if (fnPressed && (key == '.' || key == '>')) {
            // Fn+. (down arrow) - Next command (scroll down in history)
            extern int historyIndex;
            extern std::vector<String> commandHistory;
            extern String currentInput;
            if (commandHistory.size() > 0 && historyIndex >= 0) {
              historyIndex++;
              if (historyIndex >= commandHistory.size()) {
                historyIndex = -1;
                currentInput = "";
              } else {
                currentInput = commandHistory[historyIndex];
              }
              drawTerminal();
            }
          } else if (fnPressed && (key == ',' || key == '<')) {
            // Fn+, (left arrow) - Scroll output up
            scrollTerminalUp();
          } else if (fnPressed && (key == '/' || key == '?')) {
            // Fn+/ (right arrow) - Scroll output down
            scrollTerminalDown();
          } else if (key == '`') {
            // Back to apps menu
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
            currentState = APPS_MENU;
            drawScreen(true);
            return;
          } else {
            handleTerminalInput(key);
          }
        }
        if (status.enter) {
          handleTerminalInput('\n');
        }
        if (status.del) {
          handleTerminalInput('\b');
        }
        return;
      }

      // Handle Music Player app input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 6) {
        if (status.enter) {
          togglePlayPause();
        }

        for (auto key : status.word) {
          handleMusicNavigation(key);
          if (key == '+' || key == '=') {
            // Volume up
            audioVolumeUp();
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 50);
            drawMusicPlayer();
          } else if (key == '-' || key == '_') {
            // Volume down
            audioVolumeDown();
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
            drawMusicPlayer();
          } else if (key == '`') {
            // Back to music menu
            exitMusicPlayer();
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
            currentScreenNumber = 12;
            musicMenuIndex = 0;  // Set to Player position
            drawMusicMenu();
            return;
          }
        }
        return;
      }

      // Handle The Book app input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 7) {
        // Handle text input for search
        if (bookState == BOOK_SEARCH) {
          bool searchChanged = false;
          bool needsRedraw = false;

          for (auto key : status.word) {
            // Handle navigation keys
            if (key == ';' || key == ',') {
              // UP arrow - navigate results up
              if (searchResults.size() > 0 && selectedResultIndex > 0) {
                selectedResultIndex--;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
                needsRedraw = true;
              }
            } else if (key == '.' || key == '/') {
              // DOWN arrow - navigate results down (secret: / also shows TOC)
              if (key == '/' && searchInput.length() == 0) {
                // Secret: show TOC when search is empty and press /
                bookState = BOOK_TOC;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 50);
                drawTableOfContents();
                return;
              } else if (searchResults.size() > 0 && selectedResultIndex < (int)searchResults.size() - 1) {
                // Navigate results down
                selectedResultIndex++;
                if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
                needsRedraw = true;
              }
            } else if (key == '`') {
              // Back to main menu
              exitTheBook();
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              currentState = MAIN_MENU;
              drawScreen(false);
              return;
            } else if (key >= 32 && key <= 126) {
              // Regular text input (excluding special chars we already handled)
              searchInput += (char)key;
              searchChanged = true;
            }
          }

          if (status.del) {
            if (searchInput.length() > 0) {
              searchInput.remove(searchInput.length() - 1);
              searchChanged = true;
            }
          }

          // INSTANT LIVE SEARCH - run immediately as user types
          if (searchChanged) {
            // Clear old results immediately and search NOW
            searchResults.clear();
            selectedResultIndex = 0;

            if (searchInput.length() >= 1) {
              // Search all category indexes LIVE (limited to 100 lines each for speed)
              const int MAX_TOTAL_RESULTS = 30;  // Hard cap to prevent crashes

              for (int i = 0; i < totalCategories; i++) {
                // Stop if we already have enough results
                if (searchResults.size() >= MAX_TOTAL_RESULTS) {
                  break;
                }

                String indexPath = categories[i].path + "/index.txt";
                std::vector<SearchResult> catResults = searchIndex(indexPath, searchInput);

                // Add results from this category (up to the cap)
                for (size_t j = 0; j < catResults.size() && searchResults.size() < MAX_TOTAL_RESULTS; j++) {
                  searchResults.push_back(catResults[j]);
                }
              }
            }

            drawBookSearch();  // Redraw immediately with new results
          } else if (needsRedraw) {
            drawBookSearch();  // Redraw for selection change
          }

          // Enter key: go to full results page
          if (status.enter) {
            if (searchResults.size() > 0) {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
              bookState = BOOK_RESULTS;
              drawBookResults();
              return;
            }
          }

          return;
        } else if (bookState == BOOK_TOC) {
          // Handle table of contents popup
          for (auto key : status.word) {
            handleBookNavigation(key);  // Any key closes it
            if (key == '`') {
              // Back to main menu
              exitTheBook();
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              currentState = MAIN_MENU;
              drawScreen(false);
              return;
            }
          }
          return;
        } else if (bookState == BOOK_RESULTS) {
          if (status.enter) {
            // Open selected article
            if (searchResults.size() > 0) {
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
              loadArticle(searchResults[selectedResultIndex]);
            }
          }

          for (auto key : status.word) {
            handleBookNavigation(key);
            if (key == '`') {
              // Back to search
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              bookState = BOOK_SEARCH;
              drawBookSearch();
              return;
            }
          }
          return;
        } else if (bookState == BOOK_ARTICLE) {
          for (auto key : status.word) {
            handleBookNavigation(key);
            if (key == '`') {
              // Back to results
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              bookState = BOOK_RESULTS;
              drawBookResults();
              return;
            }
          }
          return;
        }
      }

      // Handle Games app input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 8) {
        for (auto key : status.word) {
          if (key == '`') {
            // Back to main menu
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
            currentState = MAIN_MENU;
            drawScreen(false);
            return;
          }
        }
        return;
      }

      // Handle Settings app input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 11) {
        if (status.enter) {
          // Handle settings selection
          if (settingsState == SETTINGS_MAIN) {
            if (settingsMenuIndex == 0) {
              // Sound toggle
              toggleSound();
              drawSettingsMenu();
            } else if (settingsMenuIndex == 1) {
              // Brightness toggle
              toggleDimMode();
              drawSettingsMenu();
            } else if (settingsMenuIndex == 2) {
              // Timezone selector
              settingsState = SETTINGS_TIMEZONE;
              drawTimezoneSelector();
            } else if (settingsMenuIndex == 3) {
              // Theme placeholder
              settingsState = SETTINGS_THEME;
              drawThemePlaceholder();
            }
          } else if (settingsState == SETTINGS_TIMEZONE) {
            // Select timezone
            if (String(timezones[timezoneSelectIndex].tzString) == "AUTO") {
              // Auto-detect timezone
              settings.timezoneAuto = true;
              autoDetectTimezone();
            } else {
              // Manual timezone selection
              settings.timezoneAuto = false;
              settings.timezoneString = timezones[timezoneSelectIndex].tzString;
              applyTimezone();
            }
            saveSettings();
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
            settingsState = SETTINGS_MAIN;
            drawSettingsMenu();
          }
        }

        for (auto key : status.word) {
          handleSettingsNavigation(key);
          if (key == '`') {
            if (settingsState == SETTINGS_MAIN) {
              // Back to main menu
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(400, 50);
              currentState = MAIN_MENU;
              drawScreen(false);
            } else {
              // Back to settings main
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(400, 50);
              settingsState = SETTINGS_MAIN;
              drawSettingsMenu();
            }
            return;
          }
        }
        return;
      }

      // Handle Music menu input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 12) {
        if (status.enter) {
          // Navigate to selected music item
          currentScreenNumber = musicMenuItems[musicMenuIndex].screenNumber;
          if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);

          // Call appropriate enter function based on screen number
          if (currentScreenNumber == 6) {
            enterMusicPlayer();
          } else if (currentScreenNumber == 4) {
            enterRadioApp();
          } else if (currentScreenNumber == 13) {
            // Music Tools
            enterMusicTools();
          }
          return;
        }

        for (auto key : status.word) {
          if (key == ';' || key == ',') {
            // Navigate up
            if (musicMenuIndex > 0) {
              musicMenuIndex--;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 50);
              drawMusicMenu();
            }
          } else if (key == '.' || key == '/') {
            // Navigate down
            if (musicMenuIndex < totalMusicItems - 1) {
              musicMenuIndex++;
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 50);
              drawMusicMenu();
            }
          } else if (key == '`') {
            // Back to apps menu
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
            currentState = APPS_MENU;
            currentAppIndex = 4;  // Set to Music position in apps
            musicMenuIndex = 0;
            drawScreen(true);
            return;
          }
        }
        return;
      }

      // Handle Music Tools input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 13) {
        if (status.enter && musicToolsState == TOOLS_MENU) {
          // Select menu item
          if (musicToolsMenuIndex == 0) {
            // Guitar Tuner
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
            startGuitarTuner();
          } else if (musicToolsMenuIndex == 1) {
            // Audio Test
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 100);
            startAudioVisualizer();
          }
        }

        for (auto key : status.word) {
          if (key == '`') {
            if (musicToolsState == GUITAR_TUNER) {
              // Stop tuner and back to tools menu
              stopGuitarTuner();
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              musicToolsState = TOOLS_MENU;
              drawMusicToolsMenu();
            } else if (musicToolsState == AUDIO_VISUALIZER) {
              // Stop visualizer and back to tools menu
              stopAudioVisualizer();
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              musicToolsState = TOOLS_MENU;
              drawMusicToolsMenu();
            } else {
              // Back to music menu
              if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
              currentScreenNumber = 12;
              drawMusicMenu();
            }
            return;
          }

          handleMusicToolsNavigation(key);
        }
        return;
      }

      // Handle Roadmap (About) page input
      if (currentState == SCREEN_VIEW && currentScreenNumber == 9) {
        for (auto key : status.word) {
          if (key == ';' || key == ',') {
            // ; or , - Scroll up
            scrollRoadmapUp();
          } else if (key == '.' || key == '/') {
            // . or / - Scroll down
            scrollRoadmapDown();
          } else if (key == '`') {
            // Back to main menu
            if (settings.soundEnabled) M5Cardputer.Speaker.tone(600, 100);
            currentState = MAIN_MENU;
            drawScreen(false);
            return;
          }
        }
        return;
      }

      // Handle password input mode
      if (currentState == WIFI_PASSWORD) {
        for (auto key : status.word) {
          if (key >= 32 && key <= 126) {
            inputPassword += (char)key;
            drawWiFiPassword();
          }
        }
        
        if (status.del) {
          if (inputPassword.length() > 0) {
            inputPassword.remove(inputPassword.length() - 1);
            drawWiFiPassword();
          }
        }
        
        if (status.enter) {
          handleSelect();
        }
        
        for (auto key : status.word) {
          if (key == '`') {
            handleBack();
            return;
          }
        }
        return;
      }
      
      // Handle delete key for WiFi scan (delete saved networks)
      if (status.del && currentState == WIFI_SCAN) {
        int numSaved = min(numSavedNetworks, 5);
        
        // Only allow deleting if a saved network is selected
        if (selectedNetworkIndex < numSaved) {
          M5Cardputer.Speaker.tone(600, 100);
          
          // Delete the selected saved network
          deleteNetwork(selectedNetworkIndex);
          
          // Adjust selection if needed
          if (selectedNetworkIndex >= numSavedNetworks && selectedNetworkIndex > 0) {
            selectedNetworkIndex--;
          }
          
          // Redraw the scan screen
          drawWiFiScan();
        }
        return; // Don't process other keys
      }

      // Normal navigation
      if (status.enter) {
        handleSelect();
      }
      
      if (status.del) {
        // Special case: delete saved network
        if (currentState == WIFI_SAVED && numSavedNetworks > 0) {
          M5Cardputer.Speaker.tone(600, 100);
          deleteNetwork(selectedSavedIndex);
          if (selectedSavedIndex >= numSavedNetworks && selectedSavedIndex > 0) {
            selectedSavedIndex--;
          }
          if (numSavedNetworks == 0) {
            currentState = MAIN_MENU;
            drawScreen(false);
          } else {
            drawWiFiSaved();
          }
        } else {
          handleBack();
        }
      }
      
      for (auto key : status.word) {
        if (key == ',' || key == ';') {
          navigateUp();
          break;
        }
        else if (key == '/' || key == '.') {
          navigateRight();
          break;
        }
        else if (key == 'g') {
          // Special case: scan from saved networks screen
          if (currentState == WIFI_SAVED) {
            M5Cardputer.Speaker.tone(1200, 100);
            scanWiFiNetworks();
          } else {
            handleSelect();
          }
          break;
        }
        else if (key == '`') {
          handleBack();
          break;
        }
      }
    }
  }
  
  // Cursor blink for password screen
  if (currentState == WIFI_PASSWORD) {
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 500) {
      lastBlink = millis();
      drawWiFiPassword();
    }
  }
  
  // Cursor blink for WiFi Fun input
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == FAKE_WIFI_INPUT) {
    static unsigned long lastBlink2 = 0;
    if (millis() - lastBlink2 > 500) {
      lastBlink2 = millis();
      drawFakeWiFiInput();
    }
  }

  // Cursor blink for Portal input
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == PORTAL_INPUT) {
    static unsigned long lastBlink3 = 0;
    if (millis() - lastBlink3 > 500) {
      lastBlink3 = millis();
      drawPortalInput();
    }
  }

  // Cursor blink for Party Time input
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == PARTY_TIME_EDIT) {
    static unsigned long lastBlink4 = 0;
    if (millis() - lastBlink4 > 500) {
      lastBlink4 = millis();
      drawPartyTimeEdit();
    }
  }

  // Animation for Fake WiFi running
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == FAKE_WIFI_RUNNING) {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 100) {
      lastUpdate = millis();
      drawFakeWiFiRunning();
    }
  }

  // Update GIF playback if in file viewer
  if (currentState == SCREEN_VIEW && currentScreenNumber == 1 && fmState == FM_FILE_VIEWER && isGifPlaying()) {
    updateGifPlayback();
  }

  // Update GIF playback for Rick Roll
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == RICKROLL_RUNNING && isGifPlaying()) {
    updateGifPlayback();
  }

  // Update Rick Roll beacon spam
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == RICKROLL_RUNNING) {
    updateRickRoll();
  }

  // Update Party Time beacon spam
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == PARTY_TIME_RUNNING) {
    updatePartyTime();
    // Periodic display update
    static unsigned long lastPartyDraw = 0;
    if (millis() - lastPartyDraw > 300) {
      lastPartyDraw = millis();
      drawPartyTimeRunning();
    }
  }

  // Update TV-B-Gone
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == TV_B_GONE_RUNNING) {
    updateTVBGone();
  }

  // Update Analytics features
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == SPEEDOMETER_RUNNING) {
    updateSpeedometer();
  }
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == PROBE_SNIFFER_RUNNING) {
    updateProbeSniffer();
  }

  // Handle Portal Games loop
  if (currentState == SCREEN_VIEW && currentScreenNumber == 2 && wifiFunState == PORTAL_GAMES_RUNNING) {
    handlePortalGamesLoop();
  }

  // Handle web server for file transfer
  // NOTE: Web server is handled by FreeRTOS background task (transferTask in background_services.cpp)
  // No need to call handleWebServerLoop() here - it would cause double-handling and crashes!

  // Update guitar tuner
  if (currentState == SCREEN_VIEW && currentScreenNumber == 13 && musicToolsState == GUITAR_TUNER) {
    updateGuitarTuner();
  }

  // Update audio visualizer
  if (currentState == SCREEN_VIEW && currentScreenNumber == 13 && musicToolsState == AUDIO_VISUALIZER) {
    updateAudioVisualizer();
  }

  // Update audio playback (centralized - handles both radio and music!)
  // This runs ALWAYS to ensure audio continues across all screens/apps/screensaver
  updateAudioIfPlaying();

  // Update star GIF animation (non-blocking)
  if (currentState == APPS_MENU || currentState == MAIN_MENU) {
    updateStarGifPlayback();
  }

  // Restore audio volume after navigation delay (unmute when idle)
  if (audioCurrentlyMuted && millis() - lastKeyPressTime > AUDIO_UNMUTE_DELAY) {
    restoreAudioVolume();
    audioCurrentlyMuted = false;
  }

  // Use shorter delay when audio is playing to keep buffer full
  if (isRadioPlaying() || isAudioPlaying()) {
    delay(1);  // Minimal delay for smooth audio
  } else {
    delay(10);  // Normal delay when no audio
  }
}