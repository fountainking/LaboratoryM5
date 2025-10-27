#include "labchat.h"
#include "security_manager.h"
#include "esp_now_manager.h"
#include "message_handler.h"
#include "ui.h"
#include <M5Cardputer.h>

// LabCHAT state
LabChatState chatState = CHAT_SETUP_PIN;
String pinInput = "";
String networkPasswordInput = "";
String networkNameInput = "";
String chatInput = "";
String usernameInput = "";
int scrollPosition = 0;
int selectedUserIndex = 0;
int chatCurrentChannel = 0;
bool chatActive = false;
unsigned long lastPresenceBroadcast = 0;

// Menu indices
int networkMenuIndex = 0;
int chatSettingsMenuIndex = 0;

// Cursor blink
bool cursorVisible = true;
unsigned long lastCursorBlink = 0;

// Forward declarations of shared functions from file_manager.cpp
extern void drawStar(int x, int y, int size, uint16_t color);
extern void drawNavHint(const char* text, int x, int y);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void drawLightningBolt(int x, int y, int size, uint16_t color) {
  // Simple lightning bolt shape ⚡
  M5Cardputer.Display.fillTriangle(x, y, x + size/2, y + size/2, x - size/3, y + size/2, color);
  M5Cardputer.Display.fillTriangle(x, y + size, x - size/2, y + size/2, x + size/3, y + size/2, color);
}

void drawLabChatHeader(const char* subtitle) {
  // Calculate header width
  String headerText = "LabCHAT";
  if (subtitle) {
    headerText += " - ";
    headerText += subtitle;
  }

  int textWidth = headerText.length() * 6;
  int totalWidth = 20 + textWidth + 30; // Lightning bolt + text + padding

  // Header rectangle (Files aesthetic)
  M5Cardputer.Display.fillRoundRect(18, 8, totalWidth, 20, 10, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(18, 8, totalWidth, 20, 10, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(19, 9, totalWidth - 2, 18, 9, TFT_BLACK);

  // Lightning bolt icon ⚡ (red/yellow gradient effect)
  drawLightningBolt(30, 18, 8, TFT_RED);

  // "LabCHAT" text
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString(headerText.c_str(), 48, 14);
}

void drawTextInputBox(const char* prompt, String& input, bool isPassword) {
  // Input box (Files aesthetic)
  M5Cardputer.Display.fillRoundRect(20, 50, 200, 60, 12, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(20, 50, 200, 60, 12, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(21, 51, 198, 58, 11, TFT_BLACK);

  // Prompt
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString(prompt, 30, 60);

  // Input field
  String displayText = input;
  if (isPassword && input.length() > 0) {
    displayText = "";
    for (int i = 0; i < input.length(); i++) {
      displayText += "*";
    }
  }

  if (displayText.length() > 28) {
    displayText = displayText.substring(displayText.length() - 28);
  }

  M5Cardputer.Display.drawString(displayText.c_str(), 30, 75);

  // Blinking cursor
  if (cursorVisible) {
    int cursorX = 30 + (displayText.length() * 6);
    M5Cardputer.Display.drawLine(cursorX, 75, cursorX, 83, TFT_BLACK);
  }

  // Nav hints
  drawNavHint("Enter=OK  Del=Erase  `=Back", 40, 95);
}

// ============================================================================
// STATE DRAWING
// ============================================================================

void drawPinSetup() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Setup");

  drawTextInputBox("Create 4-char PIN:", pinInput, true);

  // Info
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Use 0-9, A-Z, !@#$%", 45, 118);
}

void drawPinEntry() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Unlock");

  drawTextInputBox("Enter PIN:", pinInput, true);
}

void drawNetworkMenu() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Network");

  // Menu box
  M5Cardputer.Display.fillRoundRect(20, 40, 200, 70, 12, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(20, 40, 200, 70, 12, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(21, 41, 198, 68, 11, TFT_BLACK);

  const char* options[] = {"Create Network", "Join Network"};

  for (int i = 0; i < 2; i++) {
    M5Cardputer.Display.setTextSize(1);
    if (i == networkMenuIndex) {
      M5Cardputer.Display.setTextColor(TFT_WHITE);
      M5Cardputer.Display.fillRect(30, 50 + (i * 25), 180, 18, TFT_BLACK);
      M5Cardputer.Display.drawString(options[i], 40, 54 + (i * 25));
    } else {
      M5Cardputer.Display.setTextColor(TFT_BLACK);
      M5Cardputer.Display.drawString(options[i], 40, 54 + (i * 25));
    }
  }

  drawNavHint("Up/Down  Enter=Select  `=Exit", 30, 118);
}

void drawCreateNetwork() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Create");

  if (networkNameInput.length() == 0) {
    drawTextInputBox("Network Name:", networkPasswordInput, false);
  } else {
    drawTextInputBox("Password (8+ chars):", networkPasswordInput, true);
  }
}

void drawJoinNetwork() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Join");

  drawTextInputBox("Network Password:", networkPasswordInput, true);
}

void drawMainChat() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);

  // Header with channel indicator
  char subtitle[32];
  if (chatCurrentChannel == 0) {
    snprintf(subtitle, 32, "#general [%d]", espNowManager.getPeerCount());
  } else {
    snprintf(subtitle, 32, "#ch%d [%d]", chatCurrentChannel, espNowManager.getPeerCount());
  }
  drawLabChatHeader(subtitle);

  // Message area (rounded rect)
  M5Cardputer.Display.fillRoundRect(5, 32, 230, 85, 10, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(5, 32, 230, 85, 10, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(6, 33, 228, 83, 9, TFT_BLACK);

  // Display messages (8 lines visible, 10px per line)
  M5Cardputer.Display.setTextSize(1);
  int messageCount = messageHandler.getQueueCount();
  int startIndex = max(0, messageCount - 8 - scrollPosition);
  int endIndex = min(messageCount, startIndex + 8);

  for (int i = startIndex; i < endIndex; i++) {
    DisplayMessage* msg = messageHandler.getQueuedMessage(i);
    if (!msg) continue;

    int lineY = 38 + ((i - startIndex) * 10);

    // Different colors for own messages
    if (msg->isOwn) {
      M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    } else {
      M5Cardputer.Display.setTextColor(TFT_BLACK);
    }

    // Format: "username: message"
    String line = String(msg->username) + ": " + msg->content;
    if (line.length() > 38) {
      line = line.substring(0, 35) + "...";
    }

    M5Cardputer.Display.drawString(line.c_str(), 10, lineY);
  }

  // Input area
  M5Cardputer.Display.fillRoundRect(5, 120, 230, 14, 7, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(5, 120, 230, 14, 7, TFT_BLACK);

  M5Cardputer.Display.setTextColor(TFT_BLACK);
  String displayInput = chatInput;
  if (displayInput.length() > 36) {
    displayInput = displayInput.substring(displayInput.length() - 36);
  }
  M5Cardputer.Display.drawString((String("> ") + displayInput).c_str(), 10, 123);

  // Cursor
  if (cursorVisible) {
    int cursorX = 10 + ((displayInput.length() + 2) * 6);
    if (cursorX < 230) {
      M5Cardputer.Display.drawLine(cursorX, 123, cursorX, 131, TFT_BLACK);
    }
  }
}

void drawUserList() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Users");

  // User list box
  M5Cardputer.Display.fillRoundRect(20, 35, 200, 75, 12, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(20, 35, 200, 75, 12, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(21, 36, 198, 73, 11, TFT_BLACK);

  int peerCount = espNowManager.getPeerCount();

  if (peerCount == 0) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("No users online", 70, 65);
  } else {
    for (int i = 0; i < min(5, peerCount); i++) {
      PeerDevice* peer = espNowManager.getPeer(i);
      if (!peer) continue;

      M5Cardputer.Display.setTextSize(1);
      M5Cardputer.Display.setTextColor(TFT_BLACK);

      String userLine = String(peer->username) + " (" + String(peer->deviceID) + ")";
      if (userLine.length() > 30) {
        userLine = userLine.substring(0, 27) + "...";
      }

      M5Cardputer.Display.drawString(userLine.c_str(), 30, 42 + (i * 12));
    }

    if (peerCount > 5) {
      M5Cardputer.Display.setTextColor(TFT_DARKGREY);
      M5Cardputer.Display.drawString("...and more", 75, 100);
    }
  }

  drawNavHint("`=Back", 100, 118);
}

void drawChatSettings() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Settings");

  // Settings box
  M5Cardputer.Display.fillRoundRect(20, 40, 200, 75, 12, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(20, 40, 200, 75, 12, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(21, 41, 198, 73, 11, TFT_BLACK);

  const char* options[] = {"Change Username", "Network Info", "Leave Network"};

  for (int i = 0; i < 3; i++) {
    M5Cardputer.Display.setTextSize(1);
    if (i == chatSettingsMenuIndex) {
      M5Cardputer.Display.setTextColor(TFT_WHITE);
      M5Cardputer.Display.fillRect(30, 48 + (i * 20), 180, 16, TFT_BLACK);
      M5Cardputer.Display.drawString(options[i], 40, 52 + (i * 20));
    } else {
      M5Cardputer.Display.setTextColor(TFT_BLACK);
      M5Cardputer.Display.drawString(options[i], 40, 52 + (i * 20));
    }
  }

  drawNavHint("Up/Down  Enter=Select  `=Back", 30, 118);
}

void drawChannelSwitch() {
  // Overlay on main chat
  drawMainChat();

  // Channel selector overlay
  M5Cardputer.Display.fillRoundRect(60, 45, 120, 50, 10, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(60, 45, 120, 50, 10, TFT_RED);
  M5Cardputer.Display.drawRoundRect(61, 46, 118, 48, 9, TFT_RED);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("Switch Channel", 78, 52);

  M5Cardputer.Display.setTextSize(2);
  if (chatCurrentChannel == 0) {
    M5Cardputer.Display.drawString("#general", 85, 68);
  } else {
    M5Cardputer.Display.drawString(("#ch" + String(chatCurrentChannel)).c_str(), 100, 68);
  }

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("0-9 or Esc", 95, 82);
}

void drawDMSelect() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Direct Message");

  M5Cardputer.Display.fillRoundRect(20, 35, 200, 75, 12, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(20, 35, 200, 75, 12, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(21, 36, 198, 73, 11, TFT_BLACK);

  int peerCount = espNowManager.getPeerCount();

  if (peerCount == 0) {
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("No users online", 70, 65);
  } else {
    for (int i = 0; i < min(5, peerCount); i++) {
      PeerDevice* peer = espNowManager.getPeer(i);
      if (!peer) continue;

      M5Cardputer.Display.setTextSize(1);

      if (i == selectedUserIndex) {
        M5Cardputer.Display.setTextColor(TFT_WHITE);
        M5Cardputer.Display.fillRect(30, 42 + (i * 12), 180, 10, TFT_BLACK);
      } else {
        M5Cardputer.Display.setTextColor(TFT_BLACK);
      }

      String userLine = String(peer->username);
      if (userLine.length() > 28) {
        userLine = userLine.substring(0, 25) + "...";
      }
      M5Cardputer.Display.drawString(userLine.c_str(), 35, 42 + (i * 12));
    }
  }

  drawNavHint("Up/Down  Enter=Select  `=Back", 30, 118);
}

// ============================================================================
// MAIN FUNCTIONS
// ============================================================================

void enterLabChat() {
  chatActive = true;

  // Check if device PIN is set
  if (!DevicePIN::isSet()) {
    chatState = CHAT_SETUP_PIN;
    pinInput = "";
  } else {
    chatState = CHAT_ENTER_PIN;
    pinInput = "";
  }

  drawLabChat();
}

void exitLabChat() {
  chatActive = false;
  espNowManager.deinit();
}

void updateLabChat() {
  if (!chatActive) return;

  // Cursor blink
  if (millis() - lastCursorBlink > 500) {
    cursorVisible = !cursorVisible;
    lastCursorBlink = millis();
    if (chatState == CHAT_MAIN || chatState == CHAT_SETUP_PIN ||
        chatState == CHAT_ENTER_PIN || chatState == CHAT_CREATE_NETWORK ||
        chatState == CHAT_JOIN_NETWORK) {
      drawLabChat();
    }
  }

  // Broadcast presence every 30 seconds
  if (chatState == CHAT_MAIN && millis() - lastPresenceBroadcast > 30000) {
    messageHandler.sendPresence();
    lastPresenceBroadcast = millis();
  }

  // Clean up inactive peers every minute
  static unsigned long lastCleanup = 0;
  if (chatState == CHAT_MAIN && millis() - lastCleanup > 60000) {
    espNowManager.cleanupInactivePeers();
    lastCleanup = millis();
  }
}

void drawLabChat() {
  switch (chatState) {
    case CHAT_SETUP_PIN:
      drawPinSetup();
      break;
    case CHAT_ENTER_PIN:
      drawPinEntry();
      break;
    case CHAT_NETWORK_MENU:
      drawNetworkMenu();
      break;
    case CHAT_CREATE_NETWORK:
      drawCreateNetwork();
      break;
    case CHAT_JOIN_NETWORK:
      drawJoinNetwork();
      break;
    case CHAT_MAIN:
      drawMainChat();
      break;
    case CHAT_USER_LIST:
      drawUserList();
      break;
    case CHAT_SETTINGS:
      drawChatSettings();
      break;
    case CHAT_CHANNEL_SWITCH:
      drawChannelSwitch();
      break;
    case CHAT_DM_SELECT:
      drawDMSelect();
      break;
  }
}

void handleLabChatNavigation(char key) {
  // Handle based on current state
  switch (chatState) {
    case CHAT_SETUP_PIN:
    case CHAT_ENTER_PIN: {
      if (key == '\n') { // Enter
        if (pinInput.length() == 4) {
          if (chatState == CHAT_SETUP_PIN) {
            DevicePIN::create(pinInput);
            chatState = CHAT_NETWORK_MENU;
            pinInput = "";
          } else {
            if (DevicePIN::verify(pinInput)) {
              // Try to load existing network
              if (securityManager.loadFromPreferences()) {
                espNowManager.init(securityManager.getPMK());
                chatState = CHAT_MAIN;
                messageHandler.sendPresence();
                lastPresenceBroadcast = millis();
              } else {
                chatState = CHAT_NETWORK_MENU;
              }
              pinInput = "";
            } else {
              pinInput = "";
              drawLabChat();
            }
          }
        }
      } else if (key == 8 || key == 127) { // Backspace/Del
        if (pinInput.length() > 0) {
          pinInput.remove(pinInput.length() - 1);
        }
      } else if (key == '`') {
        exitLabChat();
        currentScreenNumber = 0;
        currentState = MAIN_MENU;
        extern void drawScreen(bool statusBar);
        drawScreen(true);
        return;
      } else if (pinInput.length() < 4) {
        // Validate character
        if ((key >= '0' && key <= '9') ||
            (key >= 'A' && key <= 'Z') ||
            (key >= 'a' && key <= 'z') ||
            key == '!' || key == '@' || key == '#' || key == '$' || key == '%' ||
            key == '^' || key == '&' || key == '*' || key == '(' || key == ')') {
          pinInput += key;
        }
      }
      break;
    }

    case CHAT_NETWORK_MENU: {
      if (key == ';') { // Up
        networkMenuIndex = (networkMenuIndex - 1 + 2) % 2;
      } else if (key == '.') { // Down
        networkMenuIndex = (networkMenuIndex + 1) % 2;
      } else if (key == '\n') { // Enter
        if (networkMenuIndex == 0) {
          chatState = CHAT_CREATE_NETWORK;
          networkPasswordInput = "";
          networkNameInput = "";
        } else {
          chatState = CHAT_JOIN_NETWORK;
          networkPasswordInput = "";
        }
      } else if (key == '`') {
        exitLabChat();
        extern void drawScreen(bool statusBar);
        currentScreenNumber = 0;
        currentState = MAIN_MENU;
        drawScreen(true);
        return;
      }
      break;
    }

    case CHAT_CREATE_NETWORK:
    case CHAT_JOIN_NETWORK: {
      if (key == '\n') { // Enter
        if (chatState == CHAT_CREATE_NETWORK) {
          if (networkNameInput.length() == 0) {
            // First step: get name
            networkNameInput = networkPasswordInput;
            networkPasswordInput = "";
          } else if (networkPasswordInput.length() >= 8) {
            // Second step: create network
            if (securityManager.createNetwork(networkPasswordInput, networkNameInput)) {
              espNowManager.init(securityManager.getPMK());
              chatState = CHAT_MAIN;
              messageHandler.sendPresence();
              lastPresenceBroadcast = millis();
            }
          }
        } else { // JOIN
          if (networkPasswordInput.length() >= 8) {
            if (securityManager.joinNetwork(networkPasswordInput)) {
              espNowManager.init(securityManager.getPMK());
              chatState = CHAT_MAIN;
              messageHandler.sendPresence();
              lastPresenceBroadcast = millis();
            }
          }
        }
      } else if (key == 8 || key == 127) {
        if (networkPasswordInput.length() > 0) {
          networkPasswordInput.remove(networkPasswordInput.length() - 1);
        }
      } else if (key == '`') {
        chatState = CHAT_NETWORK_MENU;
        networkPasswordInput = "";
        networkNameInput = "";
      } else if (networkPasswordInput.length() < 50) {
        networkPasswordInput += key;
      }
      break;
    }

    case CHAT_MAIN: {
      if (key == '`') {
        chatState = CHAT_SETTINGS;
        chatSettingsMenuIndex = 0;
      } else if (key >= '0' && key <= '9') {
        chatCurrentChannel = key - '0';
        scrollPosition = 0;
      } else if (key == 'u' || key == 'U') {
        chatState = CHAT_USER_LIST;
      } else if (key == 'd' || key == 'D') {
        if (espNowManager.getPeerCount() > 0) {
          chatState = CHAT_DM_SELECT;
          selectedUserIndex = 0;
        }
      } else if (key == ';') { // Up - scroll
        scrollPosition = min(scrollPosition + 1, max(0, messageHandler.getQueueCount() - 8));
      } else if (key == '.') { // Down - scroll
        scrollPosition = max(0, scrollPosition - 1);
      } else if (key == '\n') { // Enter - send
        if (chatInput.length() > 0) {
          messageHandler.sendBroadcast(chatInput.c_str(), chatCurrentChannel);
          chatInput = "";
          scrollPosition = 0;
        }
      } else if (key == 8 || key == 127) {
        if (chatInput.length() > 0) {
          chatInput.remove(chatInput.length() - 1);
        }
      } else if (chatInput.length() < 165 && key >= 32 && key <= 126) {
        chatInput += key;
      }
      break;
    }

    case CHAT_USER_LIST: {
      if (key == '`') {
        chatState = CHAT_MAIN;
      }
      break;
    }

    case CHAT_SETTINGS: {
      if (key == ';') {
        chatSettingsMenuIndex = (chatSettingsMenuIndex - 1 + 3) % 3;
      } else if (key == '.') {
        chatSettingsMenuIndex = (chatSettingsMenuIndex + 1) % 3;
      } else if (key == '\n') {
        if (chatSettingsMenuIndex == 0) {
          // Change username - TODO
        } else if (chatSettingsMenuIndex == 1) {
          // Network info - TODO
        } else if (chatSettingsMenuIndex == 2) {
          // Leave network
          securityManager.leaveNetwork();
          espNowManager.deinit();
          chatState = CHAT_NETWORK_MENU;
        }
      } else if (key == '`') {
        chatState = CHAT_MAIN;
      }
      break;
    }

    case CHAT_DM_SELECT: {
      if (key == ';') {
        int peerCount = espNowManager.getPeerCount();
        selectedUserIndex = (selectedUserIndex - 1 + peerCount) % peerCount;
      } else if (key == '.') {
        int peerCount = espNowManager.getPeerCount();
        selectedUserIndex = (selectedUserIndex + 1) % peerCount;
      } else if (key == '\n') {
        // TODO: Enter DM mode with selected user
        chatState = CHAT_MAIN;
      } else if (key == '`') {
        chatState = CHAT_MAIN;
      }
      break;
    }
  }

  drawLabChat();
}
