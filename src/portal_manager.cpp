#include "portal_manager.h"
#include "ui.h"
#include "file_manager.h"
#include "wifi_fun.h"
#include <Preferences.h>

extern Preferences preferences;
extern WiFiFunState wifiFunState;

// Portal manager globals
PortalProfile savedPortals[MAX_SAVED_PORTALS];
int numSavedPortals = 0;
int selectedPortalIndex = 0;
PortalManagerState pmState = PM_LIST;
PortalProfile currentPortal;
String portalInputBuffer = "";

// Built-in default portal HTML
const char DEFAULT_PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Laboratory</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        .container {
            max-width: 600px;
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.3);
        }
        h1 {
            font-size: 3em;
            margin: 0 0 10px 0;
            text-align: center;
        }
        h2 {
            font-size: 1.5em;
            margin: 0 0 30px 0;
            text-align: center;
            opacity: 0.9;
        }
        p {
            font-size: 1.1em;
            line-height: 1.6;
            text-align: center;
            opacity: 0.9;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Welcome</h1>
        <h2>Captive Portal</h2>
        <p>This is a demonstration captive portal created with M5Cardputer.</p>
        <p>Upload custom HTML to /portals/ on the SD card to customize this page.</p>
    </div>
</body>
</html>
)rawliteral";

void loadSavedPortals() {
  preferences.begin("portals", true);
  numSavedPortals = preferences.getInt("count", 0);

  for (int i = 0; i < numSavedPortals && i < MAX_SAVED_PORTALS; i++) {
    savedPortals[i].name = preferences.getString(("name" + String(i)).c_str(), "");
    savedPortals[i].ssid = preferences.getString(("ssid" + String(i)).c_str(), "");
    savedPortals[i].htmlPath = preferences.getString(("path" + String(i)).c_str(), "");
    savedPortals[i].useBuiltIn = preferences.getBool(("builtin" + String(i)).c_str(), true);
  }

  preferences.end();
}

void savePortal(const PortalProfile& portal) {
  // Check if portal name already exists
  for (int i = 0; i < numSavedPortals; i++) {
    if (savedPortals[i].name == portal.name) {
      // Update existing portal
      savedPortals[i] = portal;

      preferences.begin("portals", false);
      preferences.putString(("name" + String(i)).c_str(), portal.name);
      preferences.putString(("ssid" + String(i)).c_str(), portal.ssid);
      preferences.putString(("path" + String(i)).c_str(), portal.htmlPath);
      preferences.putBool(("builtin" + String(i)).c_str(), portal.useBuiltIn);
      preferences.end();
      return;
    }
  }

  // Add new portal if space available
  if (numSavedPortals < MAX_SAVED_PORTALS) {
    savedPortals[numSavedPortals] = portal;

    preferences.begin("portals", false);
    preferences.putString(("name" + String(numSavedPortals)).c_str(), portal.name);
    preferences.putString(("ssid" + String(numSavedPortals)).c_str(), portal.ssid);
    preferences.putString(("path" + String(numSavedPortals)).c_str(), portal.htmlPath);
    preferences.putBool(("builtin" + String(numSavedPortals)).c_str(), portal.useBuiltIn);
    preferences.putInt("count", numSavedPortals + 1);
    preferences.end();

    numSavedPortals++;
  }
}

void deletePortal(int index) {
  if (index < 0 || index >= numSavedPortals) return;

  // Shift portals down
  for (int i = index; i < numSavedPortals - 1; i++) {
    savedPortals[i] = savedPortals[i + 1];
  }

  numSavedPortals--;

  // Save to preferences
  preferences.begin("portals", false);
  preferences.putInt("count", numSavedPortals);
  for (int i = 0; i < numSavedPortals; i++) {
    preferences.putString(("name" + String(i)).c_str(), savedPortals[i].name);
    preferences.putString(("ssid" + String(i)).c_str(), savedPortals[i].ssid);
    preferences.putString(("path" + String(i)).c_str(), savedPortals[i].htmlPath);
    preferences.putBool(("builtin" + String(i)).c_str(), savedPortals[i].useBuiltIn);
  }
  // Clear the last slot
  preferences.remove(("name" + String(numSavedPortals)).c_str());
  preferences.remove(("ssid" + String(numSavedPortals)).c_str());
  preferences.remove(("path" + String(numSavedPortals)).c_str());
  preferences.remove(("builtin" + String(numSavedPortals)).c_str());
  preferences.end();
}

void enterPortalManager() {
  wifiFunState = PORTAL_MANAGER_ACTIVE;
  pmState = PM_LIST;
  selectedPortalIndex = 0;
  loadSavedPortals();
  drawPortalList();
}

void drawPortalList() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawStatusBar(true);

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_CYAN);
  M5Cardputer.Display.drawString("portalDECK", 60, 25);

  M5Cardputer.Display.setTextSize(1);

  if (numSavedPortals == 0) {
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("No saved portals", 70, 60);
    M5Cardputer.Display.drawString("Press Enter to create", 55, 75);
  } else {
    // Show up to 5 portals
    int startIdx = max(0, selectedPortalIndex - 2);
    int endIdx = min(numSavedPortals, startIdx + 5);

    for (int i = startIdx; i < endIdx; i++) {
      int yPos = 45 + ((i - startIdx) * 15);

      if (i == selectedPortalIndex) {
        M5Cardputer.Display.fillRoundRect(5, yPos - 2, 230, 14, 3, TFT_BLUE);
        M5Cardputer.Display.setTextColor(TFT_WHITE);
      } else {
        M5Cardputer.Display.setTextColor(TFT_LIGHTGREY);
      }

      String display = savedPortals[i].name;
      if (display.length() > 25) {
        display = display.substring(0, 25) + "...";
      }
      M5Cardputer.Display.drawString(display.c_str(), 10, yPos);
    }
  }

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString(",/=Nav Enter=Launch Del=Del", 20, 110);
  M5Cardputer.Display.drawString("n=New d=Demo u=Upload `=Back", 28, 120);
}

void drawPortalCreate() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawStatusBar(true);

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_CYAN);
  M5Cardputer.Display.drawString("New Portal", 65, 25);

  M5Cardputer.Display.setTextSize(1);

  if (pmState == PM_CREATE_NAME) {
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString("Portal Name:", 10, 50);

    // Input box
    M5Cardputer.Display.drawRect(5, 65, 230, 20, TFT_WHITE);

    if (portalInputBuffer.length() == 0) {
      M5Cardputer.Display.setTextColor(TFT_DARKGREY);
      M5Cardputer.Display.drawString("Type portal name...", 10, 70);
    } else {
      M5Cardputer.Display.setTextColor(TFT_WHITE);
      String display = portalInputBuffer;
      if (display.length() > 30) {
        display = display.substring(display.length() - 30);
      }
      M5Cardputer.Display.drawString(display.c_str(), 10, 70);
    }

    // Cursor
    if ((millis() / 500) % 2 == 0) {
      int cursorX = 10 + (min((int)portalInputBuffer.length(), 30) * 6);
      M5Cardputer.Display.drawLine(cursorX, 70, cursorX, 80, TFT_WHITE);
    }

  } else if (pmState == PM_CREATE_SSID) {
    M5Cardputer.Display.setTextColor(TFT_GREEN);
    M5Cardputer.Display.drawString("Name: " + currentPortal.name, 10, 45);

    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString("WiFi SSID:", 10, 60);

    // Input box
    M5Cardputer.Display.drawRect(5, 75, 230, 20, TFT_WHITE);

    if (portalInputBuffer.length() == 0) {
      M5Cardputer.Display.setTextColor(TFT_DARKGREY);
      M5Cardputer.Display.drawString("Type WiFi name...", 10, 80);
    } else {
      M5Cardputer.Display.setTextColor(TFT_WHITE);
      String display = portalInputBuffer;
      if (display.length() > 30) {
        display = display.substring(display.length() - 30);
      }
      M5Cardputer.Display.drawString(display.c_str(), 10, 80);
    }

    // Cursor
    if ((millis() / 500) % 2 == 0) {
      int cursorX = 10 + (min((int)portalInputBuffer.length(), 30) * 6);
      M5Cardputer.Display.drawLine(cursorX, 80, cursorX, 90, TFT_WHITE);
    }

  } else if (pmState == PM_CREATE_FILE) {
    M5Cardputer.Display.setTextColor(TFT_GREEN);
    M5Cardputer.Display.drawString("Name: " + currentPortal.name, 10, 40);
    M5Cardputer.Display.drawString("SSID: " + currentPortal.ssid, 10, 52);

    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString("HTML File Path:", 10, 67);

    // Input box
    M5Cardputer.Display.drawRect(5, 82, 230, 20, TFT_WHITE);

    if (portalInputBuffer.length() == 0) {
      M5Cardputer.Display.setTextColor(TFT_DARKGREY);
      M5Cardputer.Display.drawString("/portals/...", 10, 87);
    } else {
      M5Cardputer.Display.setTextColor(TFT_WHITE);
      String display = portalInputBuffer;
      if (display.length() > 30) {
        display = display.substring(display.length() - 30);
      }
      M5Cardputer.Display.drawString(display.c_str(), 10, 87);
    }

    // Cursor
    if ((millis() / 500) % 2 == 0) {
      int cursorX = 10 + (min((int)portalInputBuffer.length(), 30) * 6);
      M5Cardputer.Display.drawLine(cursorX, 87, cursorX, 97, TFT_WHITE);
    }
  }

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Enter=Next Del=Backspace `=Cancel", 15, 120);
}

void drawPortalUploadHelp() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawStatusBar(true);

  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextColor(TFT_YELLOW);
  M5Cardputer.Display.drawString("Upload HTML", 55, 20);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_WHITE);
  M5Cardputer.Display.drawString("1. Connect to saved WiFi", 10, 45);
  M5Cardputer.Display.drawString("2. Go to Transfer app", 10, 60);
  M5Cardputer.Display.drawString("3. Note IP address shown", 10, 75);
  M5Cardputer.Display.drawString("4. Upload HTML files to", 10, 90);
  M5Cardputer.Display.drawString("   /portals/ folder", 10, 100);

  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Press ` to go back", 60, 120);
}

String loadPortalHTML(const PortalProfile& portal) {
  if (portal.useBuiltIn || !sdCardMounted) {
    return String(DEFAULT_PORTAL_HTML);
  }

  // Try to load from SD card
  File file = SD.open(portal.htmlPath.c_str());
  if (!file) {
    // Fall back to default if file not found
    return String(DEFAULT_PORTAL_HTML);
  }

  String html = "";
  while (file.available()) {
    html += (char)file.read();
  }
  file.close();

  return html;
}

void handlePortalManagerNavigation(char key) {
  if (pmState == PM_LIST) {
    if (key == ',' || key == ';') {
      if (selectedPortalIndex > 0) {
        selectedPortalIndex--;
        drawPortalList();
      }
    } else if (key == '/' || key == '.') {
      if (selectedPortalIndex < numSavedPortals - 1) {
        selectedPortalIndex++;
        drawPortalList();
      }
    }
  }
}
