#ifndef LABCHAT_H
#define LABCHAT_H

#include <Arduino.h>
#include "config.h"

// LabCHAT states
enum LabChatState {
  CHAT_SETUP_PIN,         // First-time PIN setup
  CHAT_ENTER_PIN,         // Enter PIN to unlock
  CHAT_NETWORK_MENU,      // Create/Join network menu
  CHAT_CREATE_NETWORK,    // Create new network (password + name)
  CHAT_JOIN_NETWORK,      // Join existing network (password)
  CHAT_MAIN,              // Main chat interface
  CHAT_USER_LIST,         // Show connected users
  CHAT_SETTINGS,          // Chat settings (username, leave network)
  CHAT_CHANNEL_SWITCH,    // Channel switcher overlay
  CHAT_DM_SELECT          // Direct message user selection
};

// LabCHAT functions
void enterLabChat();
void exitLabChat();
void handleLabChatNavigation(char key);
void updateLabChat();
void drawLabChat();

// State-specific draw functions
void drawPinSetup();
void drawPinEntry();
void drawNetworkMenu();
void drawCreateNetwork();
void drawJoinNetwork();
void drawMainChat();
void drawUserList();
void drawChatSettings();
void drawChannelSwitch();
void drawDMSelect();

// Helper functions
void drawLabChatHeader(const char* subtitle = nullptr);
void drawTextInputBox(const char* prompt, String& input, bool isPassword = false);
void drawNavHint(const char* text, int x, int y);

// LabCHAT globals
extern LabChatState chatState;
extern String pinInput;
extern String networkPasswordInput;
extern String networkNameInput;
extern String chatInput;
extern String usernameInput;
extern int scrollPosition;
extern int selectedUserIndex;
extern int currentChannel;
extern bool chatActive;
extern unsigned long lastPresenceBroadcast;

#endif
