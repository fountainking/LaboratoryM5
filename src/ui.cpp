#include "ui.h"
#include "navigation.h"
#include "background_services.h"
#include <WiFi.h>

// Draw a simple emoji icon at position with optional scaling
void drawEmojiIcon(int x, int y, const char* emojiBytes, uint16_t color, int scale = 1) {
  // Match specific emojis and draw custom icons
  // Strawberry: F0 9F 8D 93
  if (memcmp(emojiBytes, "\xF0\x9F\x8D\x93", 4) == 0) {
    // Draw a simple strawberry (red circle with green top)
    int radius = 3 * scale;
    int offsetX = 4 * scale;
    int offsetY = 5 * scale;
    int leafWidth = 4 * scale;
    int leafHeight = 2 * scale;
    int leafOffsetX = 2 * scale;
    int leafOffsetY = 1 * scale;

    M5Cardputer.Display.fillCircle(x + offsetX, y + offsetY, radius, TFT_RED);
    M5Cardputer.Display.fillRect(x + leafOffsetX, y + leafOffsetY, leafWidth, leafHeight, TFT_GREEN);
    return;
  }
  // Avocado: F0 9F A5 91
  if (memcmp(emojiBytes, "\xF0\x9F\xA5\x91", 4) == 0) {
    // Draw avocado (green oval with brown center)
    int radius = 3 * scale;
    int centerRadius = 1 * scale;
    int offsetX = 4 * scale;
    int offsetY = 4 * scale;

    M5Cardputer.Display.fillCircle(x + offsetX, y + offsetY, radius, TFT_GREEN);
    M5Cardputer.Display.fillCircle(x + offsetX, y + offsetY, centerRadius, TFT_BROWN);
    return;
  }
  // Default: draw a small square for unknown emoji
  M5Cardputer.Display.fillRect(x + 2 * scale, y + 2 * scale, 5 * scale, 5 * scale, color);
}

// Helper function to render string with emoji icons
String renderWithEmojis(const String& str, int& emojiCount) {
  String result = "";
  emojiCount = 0;

  for (int i = 0; i < str.length(); i++) {
    uint8_t c = (uint8_t)str[i];

    // If it's a regular ASCII character (0-127), keep it
    if (c < 128) {
      result += (char)c;
    } else {
      // We hit a multi-byte UTF-8 sequence (emoji)
      if (c >= 0xF0 && i + 3 < str.length()) {
        // 4-byte emoji - skip it in text, we'll draw it as icon
        emojiCount++;
        result += "  "; // Leave space for icon (2 chars worth)
        i += 3;
      } else if (c >= 0xE0 && i + 2 < str.length()) {
        // 3-byte sequence
        emojiCount++;
        result += "  ";
        i += 2;
      } else if (c >= 0xC0 && i + 1 < str.length()) {
        // 2-byte sequence
        emojiCount++;
        result += "  ";
        i += 1;
      }
    }
  }

  return result;
}

// Helper function to get UTF-8 character count (not byte count)
int getUTF8Length(const String& str) {
  int count = 0;
  for (int i = 0; i < str.length(); ) {
    uint8_t c = str[i];
    if (c < 0x80) {
      i += 1; // Single-byte character (ASCII)
    } else if (c < 0xE0) {
      i += 2; // 2-byte character
    } else if (c < 0xF0) {
      i += 3; // 3-byte character (most emojis)
    } else {
      i += 4; // 4-byte character
    }
    count++;
  }
  return count;
}

// Helper function to truncate UTF-8 string by character count
String truncateUTF8(const String& str, int maxChars) {
  int count = 0;
  int bytePos = 0;

  for (int i = 0; i < str.length() && count < maxChars; ) {
    uint8_t c = str[i];
    int charBytes = 1;

    if (c < 0x80) {
      charBytes = 1;
    } else if (c < 0xE0) {
      charBytes = 2;
    } else if (c < 0xF0) {
      charBytes = 3;
    } else {
      charBytes = 4;
    }

    i += charBytes;
    bytePos = i;
    count++;
  }

  return str.substring(0, bytePos);
}

// Cached time to avoid blocking getLocalTime() calls on every draw
static String cachedTime = "--:--";
static unsigned long lastTimeUpdate = 0;
const unsigned long TIME_UPDATE_INTERVAL = 30000; // Update every 30 seconds

String getCurrentTime() {
  // Return cached time if recently updated (within 30 seconds)
  if (millis() - lastTimeUpdate < TIME_UPDATE_INTERVAL) {
    return cachedTime;
  }

  // Only update if WiFi is connected (NTP needs network)
  if (WiFi.status() != WL_CONNECTED) {
    cachedTime = "--:--";
    return cachedTime;
  }

  // Try to get time (non-blocking with timeout)
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 100)) { // 100ms timeout instead of default 5000ms
    char timeStr[6];
    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    cachedTime = String(timeStr);
    lastTimeUpdate = millis();
  }

  return cachedTime;
}

int getBatteryPercent() {
  return M5Cardputer.Power.getBatteryLevel();
}

bool isCharging() {
  return M5Cardputer.Power.isCharging();
}

void drawStar(int x, int y, int size, float angle, uint16_t fillColor, uint16_t outlineColor) {
  const int points = 5;
  const float innerRadius = size * 0.38;
  const float outerRadius = size * 0.85;
  
  int xPoints[points * 2];
  int yPoints[points * 2];
  
  for (int i = 0; i < points * 2; i++) {
    float currentAngle = angle + (i * PI / points) - PI/2;
    float radius = (i % 2 == 0) ? outerRadius : innerRadius;
    xPoints[i] = x + radius * cos(currentAngle);
    yPoints[i] = y + radius * sin(currentAngle);
  }
  
  for (int i = 0; i < points; i++) {
    int next = (i + 1) % points;
    M5Cardputer.Display.fillTriangle(
      x, y, 
      xPoints[i*2], yPoints[i*2], 
      xPoints[next*2], yPoints[next*2], 
      fillColor
    );
  }
  
  for (int thickness = 0; thickness < 3; thickness++) {
    for (int i = 0; i < points * 2; i++) {
      int next = (i + 1) % (points * 2);
      M5Cardputer.Display.drawLine(
        xPoints[i] + thickness, yPoints[i], 
        xPoints[next] + thickness, yPoints[next], 
        outlineColor
      );
      M5Cardputer.Display.drawLine(
        xPoints[i], yPoints[i] + thickness, 
        xPoints[next], yPoints[next] + thickness, 
        outlineColor
      );
    }
  }
}

void drawIndicatorDots(int currentIndex, int totalItems, bool inverted) {
  uint16_t activeDotColor = inverted ? TFT_WHITE : TFT_BLACK;
  uint16_t inactiveDotColor = inverted ? TFT_DARKGREY : TFT_LIGHTGREY;
  
  int dotSize = 4;
  int dotSpacing = 10;
  int totalWidth = (totalItems * dotSpacing) - dotSpacing;
  int startX = (240 - totalWidth) / 2;
  int y = 40;
  
  for (int i = 0; i < totalItems; i++) {
    int x = startX + (i * dotSpacing);
    if (i == currentIndex) {
      M5Cardputer.Display.fillCircle(x, y, dotSize, activeDotColor);
    } else {
      M5Cardputer.Display.fillCircle(x, y, dotSize/2, inactiveDotColor);
    }
  }
}

void drawStatusBar(bool inverted) {
#if DEBUG_ENABLE_STATUSBAR == 0
  // Minimal status bar - just a line
  uint16_t fgColor = inverted ? TFT_WHITE : TFT_BLACK;
  M5Cardputer.Display.drawLine(0, 25, 240, 25, fgColor);
  return;
#endif

  uint16_t bgColor = inverted ? TFT_BLACK : TFT_WHITE;
  uint16_t fgColor = inverted ? TFT_WHITE : TFT_BLACK;

  int wifiWidth = 110;
  M5Cardputer.Display.fillRoundRect(5, 5, wifiWidth, 18, 9, bgColor);
  for (int i = 0; i < 2; i++) {
    M5Cardputer.Display.drawRoundRect(5+i, 5+i, wifiWidth-i*2, 18-i*2, 9-i, fgColor);
  }

#if DEBUG_ENABLE_WIFI
  wifiConnected = (WiFi.status() == WL_CONNECTED);
#else
  wifiConnected = false;
#endif
  if (wifiConnected) {
    String rawSSID = WiFi.SSID();

    // Draw text with emoji icons
    M5Cardputer.Display.setTextSize(1);
    uint16_t textColor = (wifiConnected ? (inverted ? TFT_WHITE : TFT_BLACK) : TFT_RED);
    M5Cardputer.Display.setTextColor(textColor);

    int textX = 12;
    int textY = 10;

    // Parse and draw string with emojis
    for (int i = 0; i < rawSSID.length() && textX < 110; i++) {
      uint8_t c = (uint8_t)rawSSID[i];

      if (c < 128) {
        // Regular ASCII - draw it
        M5Cardputer.Display.drawChar(c, textX, textY);
        textX += 6;
      } else if (c >= 0xF0 && i + 3 < rawSSID.length()) {
        // 4-byte emoji - draw icon
        char emojiBytes[5];
        emojiBytes[0] = rawSSID[i];
        emojiBytes[1] = rawSSID[i+1];
        emojiBytes[2] = rawSSID[i+2];
        emojiBytes[3] = rawSSID[i+3];
        emojiBytes[4] = 0;

        // Debug: print the bytes to serial
        Serial.printf("Emoji bytes: %02X %02X %02X %02X\n",
                     (uint8_t)emojiBytes[0],
                     (uint8_t)emojiBytes[1],
                     (uint8_t)emojiBytes[2],
                     (uint8_t)emojiBytes[3]);

        drawEmojiIcon(textX, textY, emojiBytes, textColor);
        textX += 10; // Space for emoji icon
        i += 3;
      } else {
        // Skip unknown multi-byte sequences
        if (c >= 0xE0) i += 2;
        else if (c >= 0xC0) i += 1;
      }
    }

    wifiSSID = rawSSID; // Store for other uses
  } else {
    wifiSSID = "LAB - [Offline]";
    M5Cardputer.Display.setTextSize(1);

    // Orange-yellow gradient colors
    uint16_t colors[] = {
      0xFB00,   // Deep orange (L)
      0xFC00,   // Orange (A)
      0xFC80,   // Light orange (B)
      0xFD00,   // Orange-yellow (space)
      0xFD80,   // Bright orange (-)
      0xFE00,   // Yellow-orange (space)
      0xFE40,   // Golden ([)
      0xFD20,   // Amber (O)
      0xFC60,   // Burnt orange (f)
      0xFB80,   // Dark orange (f)
      0xFC00,   // Orange (l)
      0xFD00,   // Orange-yellow (i)
      0xFE00,   // Yellow-orange (n)
      0xFD80,   // Bright orange (e)
      0xFC80    // Light orange (])
    };

    // Draw static text with gradient
    int textX = 12;
    int textY = 10;
    for (int i = 0; i < wifiSSID.length(); i++) {
      M5Cardputer.Display.setTextColor(colors[i]);
      M5Cardputer.Display.drawChar(wifiSSID[i], textX, textY);
      textX += 6;
    }
  }
  
  M5Cardputer.Display.fillRoundRect(120, 5, 55, 18, 9, bgColor);
  for (int i = 0; i < 2; i++) {
    M5Cardputer.Display.drawRoundRect(120+i, 5+i, 55-i*2, 18-i*2, 9-i, fgColor);
  }
  M5Cardputer.Display.setTextColor(fgColor);
#if DEBUG_ENABLE_TIME
  M5Cardputer.Display.drawString(getCurrentTime().c_str(), 132, 10);
#else
  M5Cardputer.Display.drawString("--:--", 132, 10);
#endif
  
  // Battery indicator - colored based on charge level
#if DEBUG_ENABLE_BATTERY
  int voltage = M5Cardputer.Power.getBatteryVoltage();

  // Convert voltage to percentage (LiPo: 4200mV=100%, 3300mV=0%)
  int batteryPercent = map(voltage, 3300, 4200, 0, 100);
  batteryPercent = constrain(batteryPercent, 0, 100);

  // Choose color based on battery level
  uint16_t batteryColor;
  if (batteryPercent > 50) {
    batteryColor = TFT_GREEN;
  } else if (batteryPercent > 20) {
    batteryColor = TFT_YELLOW;
  } else {
    batteryColor = TFT_RED;
  }
#else
  uint16_t batteryColor = TFT_DARKGREY;  // Gray when disabled
#endif
  
  // Fill the entire battery box with the color
  M5Cardputer.Display.fillRoundRect(180, 5, 55, 18, 9, batteryColor);

  // Add outline
  for (int i = 0; i < 2; i++) {
    M5Cardputer.Display.drawRoundRect(180+i, 5+i, 55-i*2, 18-i*2, 9-i, fgColor);
  }

  // Background service indicators (colored circles between time and battery)
#if DEBUG_ENABLE_BG_SERVICES
  ServiceStatus status = getServiceStatus();
  int iconX = 175;
  int iconY = 12;

  // Show circle icons for active services
  if (status.portalRunning) {
    // Portal - Cyan circle
    M5Cardputer.Display.fillCircle(iconX, iconY, 4, TFT_CYAN);
    iconX -= 10;
  }

  if (status.fakeAPRunning) {
    // Fake WiFi - Blue circle
    M5Cardputer.Display.fillCircle(iconX, iconY, 4, TFT_BLUE);
    iconX -= 10;
  }

  if (status.transferRunning) {
    // Transfer - Green circle
    M5Cardputer.Display.fillCircle(iconX, iconY, 4, TFT_GREEN);
  }
#endif
}

void drawCard(const char* label, bool inverted) {
  int cardX = 20;
  int cardY = 50;
  int cardW = 200;
  int cardH = 50;
  
  uint16_t bgColor = inverted ? TFT_BLACK : TFT_WHITE;
  uint16_t fgColor = inverted ? TFT_WHITE : TFT_BLACK;
  
  M5Cardputer.Display.fillRoundRect(cardX, cardY, cardW, cardH, 25, bgColor);
  M5Cardputer.Display.drawRoundRect(cardX, cardY, cardW, cardH, 25, fgColor);
  M5Cardputer.Display.drawRoundRect(cardX+1, cardY+1, cardW-2, cardH-2, 24, fgColor);
  M5Cardputer.Display.drawRoundRect(cardX+2, cardY+2, cardW-4, cardH-4, 23, fgColor);
  M5Cardputer.Display.drawRoundRect(cardX+3, cardY+3, cardW-6, cardH-6, 22, fgColor);

  M5Cardputer.Display.setTextSize(3);
  M5Cardputer.Display.setTextColor(fgColor);
  int textWidth = strlen(label) * 18;
  int textX = cardX + (cardW - textWidth) / 2;
  M5Cardputer.Display.drawString(label, textX, cardY + 13);
}

void drawPlaceholderScreen(int screenNum, const char* title, bool inverted) {
  uint16_t bgColor = inverted ? TFT_BLACK : TFT_WHITE;
  uint16_t fgColor = inverted ? TFT_WHITE : TFT_BLACK;
  
  M5Cardputer.Display.fillScreen(bgColor);
  drawStatusBar(inverted);
  
  M5Cardputer.Display.setTextSize(8);
  M5Cardputer.Display.setTextColor(fgColor);
  String numStr = String(screenNum);
  int numWidth = numStr.length() * 48;
  M5Cardputer.Display.drawString(numStr.c_str(), (240 - numWidth) / 2, 40);
  
  M5Cardputer.Display.setTextSize(2);
  int titleWidth = strlen(title) * 12;
  M5Cardputer.Display.drawString(title, (240 - titleWidth) / 2, 95);
  
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Press ` to return", 60, 120);
}

void drawScreen(bool inverted) {
  uint16_t bgColor = inverted ? TFT_BLACK : TFT_WHITE;
  uint16_t fgColor = inverted ? TFT_WHITE : TFT_BLACK;

  // Clear regions selectively to avoid flashing the star area
  // Clear top area (status bar)
  M5Cardputer.Display.fillRect(0, 0, 240, 28, bgColor);
  // Clear middle area (dots)
  M5Cardputer.Display.fillRect(0, 28, 240, 20, bgColor);
  // Clear card area
  M5Cardputer.Display.fillRect(0, 48, 240, 60, bgColor);
  // Clear bottom - full clear to avoid colored bars from flash animations
  M5Cardputer.Display.fillRect(0, 108, 240, 27, bgColor);  // Clear entire bottom area including star

  drawStatusBar(inverted);

  if (currentState == APPS_MENU) {
    drawCard(apps[currentAppIndex].name.c_str(), inverted);
    drawIndicatorDots(currentAppIndex, totalApps, inverted);
    // Draw static star (will be overwritten during navigation animations)
    drawStillStar();
  } else if (currentState == MAIN_MENU) {
    drawCard(mainItems[currentMainIndex].name.c_str(), inverted);
    drawIndicatorDots(currentMainIndex, totalMainItems, inverted);
    // Draw static star (will be overwritten during navigation animations)
    drawStillStar();
  }
}

void drawWiFiScan() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawStatusBar(false);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Join Wi-Fi", 90, 30);

  // Calculate total items and scrolling window
  int totalItems = numSavedNetworks + numNetworks;
  const int maxVisible = 6; // Show up to 6 networks at once
  int startIndex = 0;

  // Center the selected item in the visible window
  if (totalItems > maxVisible) {
    startIndex = selectedNetworkIndex - (maxVisible / 2);
    if (startIndex < 0) startIndex = 0;
    if (startIndex > totalItems - maxVisible) startIndex = totalItems - maxVisible;
  }

  int yPos = 50;
  int endIndex = min(startIndex + maxVisible, totalItems);

  // Show scanning message if no networks yet
  if (totalItems == 0) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_BLUE);
    M5Cardputer.Display.drawString("Scanning networks...", 50, yPos + 10);

    int dots = (millis() / 300) % 4;
    for (int i = 0; i < dots; i++) {
      M5Cardputer.Display.fillCircle(155 + (i * 8), yPos + 22, 2, TFT_BLUE);
    }
  } else {
    // Draw networks in the visible window
    for (int idx = startIndex; idx < endIndex; idx++) {
      // Draw separator line between saved and scanned networks
      if (idx == numSavedNetworks && numSavedNetworks > 0 && idx >= startIndex) {
        M5Cardputer.Display.drawLine(10, yPos - 4, 230, yPos - 4, TFT_LIGHTGREY);
        yPos += 4; // Add some spacing after the line
      }

      bool isSelected = (idx == selectedNetworkIndex);

      if (isSelected) {
        M5Cardputer.Display.fillRoundRect(5, yPos - 2, 230, 18, 3, TFT_LIGHTGREY);
      }

      M5Cardputer.Display.setTextSize(2);
      M5Cardputer.Display.setTextColor(TFT_BLACK);

      String displaySSID;
      bool isSaved = false;
      int signalStrength = 0;

      // Determine if this is a saved or scanned network
      if (idx < numSavedNetworks) {
        // Saved network
        displaySSID = savedSSIDs[idx];
        isSaved = true;
      } else {
        // Scanned network
        int scannedIdx = idx - numSavedNetworks;
        displaySSID = scannedNetworks[scannedIdx];
        signalStrength = scannedRSSI[scannedIdx];

        // Check if this scanned network is also saved
        for (int j = 0; j < numSavedNetworks; j++) {
          if (savedSSIDs[j] == displaySSID) {
            isSaved = true;
            break;
          }
        }
      }

      // Draw SSID with emoji support (like status bar)
      int textX = 15;
      int charCount = 0;
      const int maxChars = 18;  // Fewer chars at size 2

      for (int i = 0; i < displaySSID.length() && charCount < maxChars; i++) {
        uint8_t c = (uint8_t)displaySSID[i];

        if (c < 128 && textX < 200) {
          // Regular ASCII - draw it
          M5Cardputer.Display.drawChar(c, textX, yPos);
          textX += 12;  // Wider spacing for size 2
          charCount++;
        } else if (c >= 0xF0 && i + 3 < displaySSID.length() && textX < 200) {
          // 4-byte emoji - draw icon (scaled up)
          char emojiBytes[5];
          emojiBytes[0] = displaySSID[i];
          emojiBytes[1] = displaySSID[i+1];
          emojiBytes[2] = displaySSID[i+2];
          emojiBytes[3] = displaySSID[i+3];
          emojiBytes[4] = 0;

          drawEmojiIcon(textX, yPos, emojiBytes, TFT_BLACK, 2);  // Scale 2x for WiFi list
          textX += 18; // Space for emoji icon (scaled)
          i += 3;
          charCount++;
        } else {
          // Skip unknown multi-byte sequences
          if (c >= 0xE0 && i + 2 < displaySSID.length()) i += 2;
          else if (c >= 0xC0 && i + 1 < displaySSID.length()) i += 1;
        }
      }

      // Draw indicators
      if (idx < numSavedNetworks) {
        // Saved indicator (green dot)
        M5Cardputer.Display.fillCircle(230, yPos + 6, 3, TFT_GREEN);
      } else {
        // Signal bars for scanned networks
        int bars = map(signalStrength, -100, -50, 1, 4);
        for (int b = 0; b < bars; b++) {
          M5Cardputer.Display.fillRect(215 + (b * 4), yPos + 12 - (b * 2), 3, 2 + (b * 2), isSaved ? TFT_GREEN : TFT_BLUE);
        }
      }

      yPos += 18;  // More spacing for size 2
    }

    // Show scroll indicators if there are more networks
    if (startIndex > 0) {
      // Up arrow
      M5Cardputer.Display.fillTriangle(120, 48, 115, 52, 125, 52, TFT_DARKGREY);
    }
    if (endIndex < totalItems) {
      // Down arrow
      M5Cardputer.Display.fillTriangle(120, 100, 115, 96, 125, 96, TFT_DARKGREY);
    }

    // Show "Scanning..." if we have saved networks but no scanned networks yet
    if (numSavedNetworks > 0 && numNetworks == 0) {
      M5Cardputer.Display.setTextSize(1);
      M5Cardputer.Display.setTextColor(TFT_BLUE);
      M5Cardputer.Display.drawString("Scanning...", 85, yPos + 5);
    }
  }

}

void drawWiFiPassword() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawStatusBar(false);
  
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Connect to:", 50, 30);
  
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLUE);
  String displaySSID = targetSSID;
  if (displaySSID.length() > 30) {
    displaySSID = displaySSID.substring(0, 30) + "...";
  }
  M5Cardputer.Display.drawString(displaySSID.c_str(), 10, 50);
  
  M5Cardputer.Display.drawRect(5, 70, 230, 20, TFT_BLACK);
  M5Cardputer.Display.drawRect(6, 71, 228, 18, TFT_BLACK);
  
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  String displayPW = inputPassword;
  if (displayPW.length() > 30) {
    displayPW = displayPW.substring(displayPW.length() - 30);
  }
  M5Cardputer.Display.drawString(displayPW.c_str(), 10, 75);
  
  if ((millis() / 500) % 2 == 0) {
    int cursorX = 10 + (displayPW.length() * 6);
    M5Cardputer.Display.drawLine(cursorX, 75, cursorX, 85, TFT_BLACK);
  }
  
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Enter=Connect `=Back Del=Delete", 20, 110);
}

void drawWiFiSaved() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawStatusBar(false);
  
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Saved Networks", 35, 30);
  
  if (numSavedNetworks == 0) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("No saved networks", 60, 70);
  } else {
    for (int i = 0; i < numSavedNetworks; i++) {
      int yPos = 50 + (i * 15);
      
      if (i == selectedSavedIndex) {
        M5Cardputer.Display.fillRoundRect(5, yPos - 2, 230, 14, 5, TFT_LIGHTGREY);
      }
      
      M5Cardputer.Display.setTextSize(1);
      M5Cardputer.Display.setTextColor(TFT_BLACK);
      
      String displaySSID = savedSSIDs[i];
      if (displaySSID.length() > 28) {
        displaySSID = displaySSID.substring(0, 28) + "...";
      }
      M5Cardputer.Display.drawString(displaySSID.c_str(), 10, yPos);
      
      // Show saved indicator
      M5Cardputer.Display.fillCircle(225, yPos + 5, 3, TFT_GREEN);
    }
  }
  
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString(",/=Nav Enter=Connect Del=Forget", 15, 120);
  M5Cardputer.Display.drawString("`=Back", 95, 110);
}

// Music Menu
struct MusicMenuItem {
  String name;
  uint16_t color;
  int screenNumber;
};

extern MusicMenuItem musicMenuItems[];
extern int totalMusicItems;
extern int musicMenuIndex;

void drawMusicMenu() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawStatusBar(false);

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_MAGENTA);
  M5Cardputer.Display.drawString("Music", 90, 25);

  // Draw menu items
  for (int i = 0; i < totalMusicItems; i++) {
    int yPos = 50 + (i * 20);

    if (i == musicMenuIndex) {
      M5Cardputer.Display.fillRoundRect(5, yPos - 2, 230, 18, 5, TFT_DARKGREY);
    }

    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(musicMenuItems[i].color);
    M5Cardputer.Display.drawString("> " + musicMenuItems[i].name, 10, yPos);
  }

  // Instructions
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString(",/=Navigate  Enter=Select", 30, 115);
  M5Cardputer.Display.drawString("`=Back", 95, 125);
}