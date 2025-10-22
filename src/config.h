#ifndef CONFIG_H
#define CONFIG_H

#include <M5Cardputer.h>

// Version
#define FIRMWARE_VERSION "v0.08g(3)"

// Constants
#define MAX_SAVED_NETWORKS 5

// SD card (M5 Cardputer)
#define SD_SPI_SCK_PIN   40
#define SD_SPI_MISO_PIN  39
#define SD_SPI_MOSI_PIN  14
#define SD_SPI_CS_PIN    12
#define SD_SPI_FREQ      25000000

// App info structure
struct AppInfo {
  String name;
  uint16_t starColor;
  int screenNumber;
};

// Main menu item structure
struct MainItemInfo {
  String name;
  uint16_t starColor;
  int screenNumber;
};

// Menu states
enum MenuState {
  STAR_LANDING_PAGE,
  MAIN_MENU,
  APPS_MENU,
  SCREEN_VIEW,
  WIFI_SCAN,
  WIFI_PASSWORD,
  WIFI_SAVED
};

// Global app definitions (defined in main.cpp)
extern AppInfo apps[];
extern MainItemInfo mainItems[];
extern const int totalApps;
extern const int totalMainItems;

// Global state variables (defined in main.cpp)
extern MenuState currentState;
extern int currentMainIndex;
extern int currentAppIndex;
extern int currentScreenNumber;
extern float starAngle;

// WiFi globals (defined in main.cpp)
extern String scannedNetworks[];
extern int scannedRSSI[];
extern int numNetworks;
extern int selectedNetworkIndex;
extern String targetSSID;
extern String inputPassword;
extern String wifiSSID;
extern bool wifiConnected;

// Time globals (defined in main.cpp)
extern bool timeIsSynced;
extern unsigned long lastSyncTime;

// Saved networks (defined in main.cpp)
extern String savedSSIDs[MAX_SAVED_NETWORKS];
extern String savedPasswords[MAX_SAVED_NETWORKS];
extern int numSavedNetworks;
extern int selectedSavedIndex;

#endif