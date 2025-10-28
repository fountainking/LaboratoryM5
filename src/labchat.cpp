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
String channelNameInput = "";
int scrollPosition = 0;
int selectedUserIndex = 0;
int chatCurrentChannel = 0;
bool chatActive = false;
unsigned long lastPresenceBroadcast = 0;
String dmTargetID = "";
String dmTargetUsername = "";
bool hasUnreadMessages = false;

// Channel names
String channelNames[10] = {
  "general", "random", "dev", "music",
  "games", "tech", "art", "memes",
  "study", "chill"
};

// Menu indices
int networkMenuIndex = 0;
int chatSettingsMenuIndex = 0;

// Cursor blink
bool cursorVisible = true;
unsigned long lastCursorBlink = 0;

// Forward declarations of shared functions
extern void drawStar(int x, int y, int size, uint16_t color);
extern void drawNavHint(const char* text, int x, int y);
extern void drawEmojiIcon(int x, int y, const char* emoji, uint16_t color, int size);
extern uint16_t interpolateColor(uint16_t color1, uint16_t color2, float t);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void drawLabChatHeader(const char* subtitle) {
  // Calculate header width
  String headerText = "LabCHAT";
  if (subtitle) {
    headerText += " - ";
    headerText += subtitle;
  }

  int textWidth = headerText.length() * 6;
  int totalWidth = 20 + textWidth + 30; // Emoji + text + padding

  // Header rectangle (left aligned at x=5)
  M5Cardputer.Display.fillRoundRect(5, 8, totalWidth, 20, 10, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(5, 8, totalWidth, 20, 10, TFT_BLACK);

  // Strawberry emoji 🍓
  drawEmojiIcon(11, 11, "\xF0\x9F\x8D\x93", TFT_RED, 1);

  // "LabCHAT" text
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString("LabCHAT", 29, 14);

  // Subtitle (no gradient on header)
  if (subtitle) {
    M5Cardputer.Display.drawString(" - ", 29 + 42, 14);
    M5Cardputer.Display.drawString(subtitle, 29 + 42 + 18, 14);
  }
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

  // Header with channel name and DM indicator
  char subtitle[64];
  if (dmTargetID.length() > 0) {
    snprintf(subtitle, 64, "DM:%s [%d]", dmTargetUsername.c_str(), espNowManager.getPeerCount());
  } else {
    String channelName = channelNames[chatCurrentChannel];
    snprintf(subtitle, 64, "#%s [%d]", channelName.c_str(), espNowManager.getPeerCount());
  }
  drawLabChatHeader(subtitle);

  // Message area (with margin for input box)
  M5Cardputer.Display.fillRoundRect(5, 32, 230, 70, 10, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(5, 32, 230, 70, 10, TFT_ORANGE);

  // Display messages with wrapping (4 messages visible with 2-line support)
  M5Cardputer.Display.setTextSize(1);
  int messageCount = messageHandler.getQueueCount();

  // Filter messages by channel or DM mode - build filtered list first
  int filteredIndices[MESSAGE_QUEUE_SIZE];
  int filteredCount = 0;

  for (int i = 0; i < messageCount; i++) {
    DisplayMessage* msg = messageHandler.getQueuedMessage(i);
    if (!msg) continue;

    // Filter: if in DM mode, only show DMs with target, otherwise show channel messages
    bool shouldDisplay = false;
    if (dmTargetID.length() > 0) {
      // DM mode: show only direct messages with this user
      shouldDisplay = (msg->type == MSG_DIRECT);
    } else {
      // Channel mode: show only broadcasts on current channel
      shouldDisplay = (msg->type == MSG_BROADCAST && msg->channel == chatCurrentChannel);
    }

    if (shouldDisplay) {
      filteredIndices[filteredCount++] = i;
    }
  }

  // Display last 4 messages (newest at bottom), accounting for scroll
  int lineY = 40;
  int displayedCount = 0;
  int startIndex = max(0, filteredCount - 4 - scrollPosition);
  int endIndex = min(filteredCount, startIndex + 4);

  for (int idx = startIndex; idx < endIndex; idx++) {
    int i = filteredIndices[idx];
    DisplayMessage* msg = messageHandler.getQueuedMessage(i);
    if (!msg) continue;

    // Get solid user color from device ID hash (fire colors for black bg)
    uint32_t deviceHash = 0;
    for (int j = 0; j < strlen(msg->deviceID); j++) {
      deviceHash = deviceHash * 31 + msg->deviceID[j];
    }
    uint16_t userColors[] = {
      0xFFE0,  // Yellow
      0xFD20,  // Orange
      0xF800,  // Red
      0x07FF,  // Baby Blue (Cyan)
      0x001F   // Blue
    };
    uint16_t userColor = userColors[deviceHash % 5];

    // Reverse gradient colors for message content
    uint16_t gradientColors[] = {
      0xFFFF,  // White
      0x07FF,  // Cyan
      0x001F,  // Blue
      0x780F,  // Purple
      0xF800,  // Red
      0xFD20,  // Orange
      0xFFE0,  // Yellow
    };
    int numColors = 7;

    auto getMessageGradientColor = [&](int charIndex) -> uint16_t {
      int cycleLength = 80;
      int posInCycle = charIndex % cycleLength;
      float t;

      if (posInCycle < 40) {
        t = (float)posInCycle / 40.0f;
      } else {
        t = (float)(80 - posInCycle) / 40.0f;
      }

      float colorPosition = t * (numColors - 1);
      int colorIndex1 = (int)colorPosition;
      int colorIndex2 = (colorIndex1 + 1) % numColors;
      float blend = colorPosition - colorIndex1;

      return interpolateColor(gradientColors[colorIndex1], gradientColors[colorIndex2], blend);
    };

    String username = String(msg->username) + ": ";
    String content = msg->content;

    // Draw username with solid color, size 1
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(userColor);
    int xPos = 10;
    M5Cardputer.Display.drawString(username.c_str(), xPos, lineY);
    xPos += username.length() * 6;

    // Peppermint pattern - alternate red/white per message
    uint16_t messageColor = (i % 2 == 0) ? TFT_RED : TFT_WHITE;

    // Draw message content in solid color (peppermint), size 1
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(messageColor);
    int remainingChars = 38 - username.length();
    if (content.length() <= remainingChars) {
      M5Cardputer.Display.drawString(content.c_str(), xPos, lineY);
      lineY += 10;
    } else {
      // Split into two lines
      String line1 = content.substring(0, remainingChars);
      String line2 = content.substring(remainingChars);
      if (line2.length() > 38) {
        line2 = line2.substring(0, 35) + "...";
      }
      M5Cardputer.Display.drawString(line1.c_str(), xPos, lineY);
      lineY += 8;
      M5Cardputer.Display.drawString(line2.c_str(), 10, lineY);
      lineY += 10;
    }

    displayedCount++;
  }

  // Input area (black background with yellow outline, terminal style, with margins)
  M5Cardputer.Display.fillRoundRect(5, 107, 230, 23, 8, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(5, 107, 230, 23, 8, TFT_YELLOW);

  // Terminal gradient colors (smooth ping-pong)
  uint16_t gradientColors[] = {
    0xFFE0,  // Yellow
    0xFD20,  // Orange
    0xF800,  // Red
    0x780F,  // Purple
    0x001F,  // Blue
    0x07FF,  // Cyan
    0xFFFF,  // White
  };
  int numColors = 7;

  // Smooth gradient helper (from terminal)
  auto getInputGradientColor = [&](int charIndex) -> uint16_t {
    int cycleLength = 80;
    int posInCycle = charIndex % cycleLength;
    float t;

    if (posInCycle < 40) {
      t = (float)posInCycle / 40.0f;
    } else {
      t = (float)(80 - posInCycle) / 40.0f;
    }

    float colorPosition = t * (numColors - 1);
    int colorIndex1 = (int)colorPosition;
    int colorIndex2 = (colorIndex1 + 1) % numColors;
    float blend = colorPosition - colorIndex1;

    return interpolateColor(gradientColors[colorIndex1], gradientColors[colorIndex2], blend);
  };

  // Draw prompt "> " and input with smooth gradient
  String fullInput = String("> ") + chatInput;
  String displayInput = fullInput;
  int maxChars = 18;  // Reduced for larger text

  if (displayInput.length() > maxChars) {
    displayInput = displayInput.substring(displayInput.length() - maxChars);
  }

  int xPos = 10;
  int inputY = 112;  // Text Y position (5px from top of input box at y=107)
  for (int i = 0; i < displayInput.length(); i++) {
    int actualIndex = (fullInput.length() > maxChars) ? (fullInput.length() - maxChars + i) : i;
    M5Cardputer.Display.setTextSize(2);  // Changed from 1 to 2
    M5Cardputer.Display.setTextColor(getInputGradientColor(actualIndex));
    M5Cardputer.Display.drawString(String(displayInput[i]).c_str(), xPos, inputY);
    xPos += 12;  // Doubled character spacing
  }

  // Cursor
  if (cursorVisible) {
    int cursorX = 10 + (displayInput.length() * 12);  // Doubled spacing
    if (cursorX < 230) {
      M5Cardputer.Display.fillRect(cursorX, inputY, 12, 16, getInputGradientColor(fullInput.length()));  // Doubled cursor size
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

  // Settings box (taller for 4 items)
  M5Cardputer.Display.fillRoundRect(20, 35, 200, 85, 12, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(20, 35, 200, 85, 12, TFT_BLACK);
  M5Cardputer.Display.drawRoundRect(21, 36, 198, 83, 11, TFT_BLACK);

  const char* options[] = {"Change Username", "Switch Channel: ", "Network Info", "Leave Network"};

  for (int i = 0; i < 4; i++) {
    M5Cardputer.Display.setTextSize(1);
    if (i == chatSettingsMenuIndex) {
      M5Cardputer.Display.setTextColor(TFT_WHITE);
      M5Cardputer.Display.fillRect(30, 43 + (i * 18), 180, 14, TFT_BLACK);
      if (i == 1) {
        // Switch Channel - show current channel number
        String channelText = String(options[i]) + String(chatCurrentChannel);
        M5Cardputer.Display.drawString(channelText.c_str(), 40, 46 + (i * 18));
      } else {
        M5Cardputer.Display.drawString(options[i], 40, 46 + (i * 18));
      }
    } else {
      M5Cardputer.Display.setTextColor(TFT_BLACK);
      if (i == 1) {
        // Switch Channel - show current channel number
        String channelText = String(options[i]) + String(chatCurrentChannel);
        M5Cardputer.Display.drawString(channelText.c_str(), 40, 46 + (i * 18));
      } else {
        M5Cardputer.Display.drawString(options[i], 40, 46 + (i * 18));
      }
    }
  }

  drawNavHint("Up/Down  Enter/Left/Right  `=Back", 20, 118);
}

void drawChannelSwitch() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Switch Channel");

  // Channel selector box
  M5Cardputer.Display.fillRoundRect(40, 45, 160, 50, 10, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(40, 45, 160, 50, 10, TFT_RED);
  M5Cardputer.Display.drawRoundRect(41, 46, 158, 48, 9, TFT_RED);

  M5Cardputer.Display.setTextSize(2);
  String channelName = channelNames[chatCurrentChannel];
  M5Cardputer.Display.setTextColor(TFT_BLACK);
  M5Cardputer.Display.drawString(("#" + channelName).c_str(), 70, 60);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("< Up/Down >", 85, 105);

  drawNavHint("`=Back  Enter=Select", 60, 118);
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

void drawNetworkInfo() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Network Info");

  M5Cardputer.Display.fillRoundRect(20, 35, 200, 75, 12, TFT_WHITE);
  M5Cardputer.Display.drawRoundRect(20, 35, 200, 75, 12, TFT_BLACK);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_BLACK);

  String networkName = securityManager.getNetworkName();
  String deviceID = securityManager.getDeviceID();
  int peerCount = espNowManager.getPeerCount();

  M5Cardputer.Display.drawString(("Network: " + networkName).c_str(), 30, 45);
  M5Cardputer.Display.drawString(("Device: " + deviceID).c_str(), 30, 60);
  M5Cardputer.Display.drawString(("Peers: " + String(peerCount)).c_str(), 30, 75);

  drawNavHint("`=Back", 100, 118);
}

void drawChangeUsername() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  drawLabChatHeader("Username");

  drawTextInputBox("New Username:", usernameInput, false);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Max 15 characters", 70, 118);
}

void drawRenameChannel() {
  M5Cardputer.Display.fillScreen(TFT_WHITE);
  char subtitle[32];
  snprintf(subtitle, 32, "Rename #%s", channelNames[chatCurrentChannel].c_str());
  drawLabChatHeader(subtitle);

  drawTextInputBox("Channel Name:", channelNameInput, false);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(TFT_DARKGREY);
  M5Cardputer.Display.drawString("Max 15 characters", 70, 118);
}

// ============================================================================
// MAIN FUNCTIONS
// ============================================================================

// Callback for real-time message display updates
void onMessageReceived() {
  if (chatActive && chatState == CHAT_MAIN) {
    scrollPosition = 0; // Auto-scroll to latest message
    drawLabChat();
  } else {
    // Not in chat - set unread flag and beep
    hasUnreadMessages = true;
    M5Cardputer.Speaker.tone(1000, 100); // 1kHz beep for 100ms
  }
}

void enterLabChat() {
  chatActive = true;
  hasUnreadMessages = false; // Clear notification when entering chat

  // Register callback for real-time display updates
  messageHandler.setMessageCallback(onMessageReceived);

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
  // Keep ESP-NOW running in background to receive messages
  // Only deinit when explicitly leaving network in settings
}

void updateLabChat() {
  if (!chatActive) return;

  // Cursor blink (don't redraw entire screen, just toggle cursor state)
  if (millis() - lastCursorBlink > 500) {
    cursorVisible = !cursorVisible;
    lastCursorBlink = millis();
    // Don't call drawLabChat() here - causes constant flickering
    // The cursor is drawn as part of the normal draw cycle when input changes
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
    case CHAT_NETWORK_INFO:
      drawNetworkInfo();
      break;
    case CHAT_CHANGE_USERNAME:
      drawChangeUsername();
      break;
    case CHAT_RENAME_CHANNEL:
      drawRenameChannel();
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
              // Always go to network menu - no auto-connect
              chatState = CHAT_NETWORK_MENU;
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
      // Text input mode - handle typing first
      if (key == '\n') { // Enter - send message
        if (chatInput.length() > 0) {
          if (dmTargetID.length() > 0) {
            // Send DM
            messageHandler.sendDirect(dmTargetID.c_str(), chatInput.c_str());
          } else {
            // Send broadcast
            messageHandler.sendBroadcast(chatInput.c_str(), chatCurrentChannel);
          }
          chatInput = "";
          scrollPosition = 0;
        }
      } else if (key == 8 || key == 127) { // Backspace
        if (chatInput.length() > 0) {
          chatInput.remove(chatInput.length() - 1);
        }
      } else if (chatInput.length() > 0 && key >= 32 && key <= 126) {
        // Typing mode: add character, ignore hotkeys
        chatInput += key;
      } else if (chatInput.length() == 0) {
        // Navigation mode: only process hotkeys when NOT typing
        if (key == '`') {
          chatState = CHAT_SETTINGS;
          chatSettingsMenuIndex = 0;
        } else if (key == 9) { // Tab - DM hotkey
          if (espNowManager.getPeerCount() > 0) {
            chatState = CHAT_DM_SELECT;
            selectedUserIndex = 0;
          }
        } else if (key == '#') { // Rename current channel
          channelNameInput = channelNames[chatCurrentChannel];
          chatState = CHAT_RENAME_CHANNEL;
        } else if (key == 27) { // ESC - exit DM mode
          dmTargetID = "";
          dmTargetUsername = "";
        } else if (key == ';') { // Up - scroll
          scrollPosition++;
        } else if (key == '.') { // Down - scroll
          scrollPosition = max(0, scrollPosition - 1);
        } else if (key >= 32 && key <= 126) {
          // Start typing
          chatInput += key;
        }
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
        chatSettingsMenuIndex = (chatSettingsMenuIndex - 1 + 4) % 4;
      } else if (key == '.') {
        chatSettingsMenuIndex = (chatSettingsMenuIndex + 1) % 4;
      } else if (key == ',' && chatSettingsMenuIndex == 1) {
        // Left arrow - decrement channel
        chatCurrentChannel = (chatCurrentChannel - 1 + 10) % 10;
        scrollPosition = 0;
      } else if (key == '/' && chatSettingsMenuIndex == 1) {
        // Right arrow - increment channel
        chatCurrentChannel = (chatCurrentChannel + 1) % 10;
        scrollPosition = 0;
      } else if (key == '\n') {
        if (chatSettingsMenuIndex == 0) {
          // Change username
          Preferences prefs;
          prefs.begin("labchat", true);
          String defaultUsername = "User";
          const char* devID = securityManager.getDeviceID();
          int userNum = 0;
          int len = strlen(devID);
          if (len >= 4) {
            for (int i = len - 4; i < len; i++) {
              userNum = userNum * 10 + (devID[i] % 10);
            }
          }
          defaultUsername += String(userNum % 10000);
          usernameInput = prefs.getString("username", defaultUsername);
          prefs.end();
          chatState = CHAT_CHANGE_USERNAME;
        } else if (chatSettingsMenuIndex == 1) {
          // Switch channel - just go back to main (channel already changed with arrows)
          chatState = CHAT_MAIN;
        } else if (chatSettingsMenuIndex == 2) {
          // Network info
          chatState = CHAT_NETWORK_INFO;
        } else if (chatSettingsMenuIndex == 3) {
          // Leave network - clear and go to network menu
          messageHandler.clearQueue();
          securityManager.leaveNetwork();
          espNowManager.deinit();
          chatState = CHAT_NETWORK_MENU;
        }
      } else if (key == '`') {
        chatState = CHAT_NETWORK_MENU;
      }
      break;
    }

    case CHAT_CHANNEL_SWITCH: {
      if (key == ';') { // Up
        chatCurrentChannel = (chatCurrentChannel - 1 + 10) % 10;
        scrollPosition = 0;
      } else if (key == '.') { // Down
        chatCurrentChannel = (chatCurrentChannel + 1) % 10;
        scrollPosition = 0;
      } else if (key == '\n') { // Enter - confirm
        chatState = CHAT_SETTINGS;
      } else if (key == '`') {
        chatState = CHAT_SETTINGS;
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
        // Enter DM mode with selected user
        PeerDevice* peer = espNowManager.getPeer(selectedUserIndex);
        if (peer) {
          dmTargetID = String(peer->deviceID);
          dmTargetUsername = String(peer->username);
        }
        chatState = CHAT_MAIN;
      } else if (key == '`') {
        chatState = CHAT_MAIN;
      }
      break;
    }

    case CHAT_NETWORK_INFO: {
      if (key == '`') {
        chatState = CHAT_SETTINGS;
      }
      break;
    }

    case CHAT_CHANGE_USERNAME: {
      if (key == '\n') {
        if (usernameInput.length() > 0 && usernameInput.length() <= 15) {
          Preferences prefs;
          prefs.begin("labchat", false);
          prefs.putString("username", usernameInput);
          prefs.end();
          chatState = CHAT_SETTINGS;
        }
      } else if (key == 8 || key == 127) {
        if (usernameInput.length() > 0) {
          usernameInput.remove(usernameInput.length() - 1);
        }
      } else if (key == '`') {
        chatState = CHAT_SETTINGS;
      } else if (usernameInput.length() < 15 && key >= 32 && key <= 126) {
        usernameInput += key;
      }
      break;
    }

    case CHAT_RENAME_CHANNEL: {
      if (key == '\n') {
        if (channelNameInput.length() > 0 && channelNameInput.length() <= 15) {
          channelNames[chatCurrentChannel] = channelNameInput;
          chatState = CHAT_MAIN;
        }
      } else if (key == 8 || key == 127) {
        if (channelNameInput.length() > 0) {
          channelNameInput.remove(channelNameInput.length() - 1);
        }
      } else if (key == '`') {
        chatState = CHAT_MAIN;
      } else if (channelNameInput.length() < 15 && key >= 32 && key <= 126) {
        channelNameInput += key;
      }
      break;
    }
  }

  drawLabChat();
}
