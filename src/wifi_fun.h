#ifndef WIFI_FUN_H
#define WIFI_FUN_H

#include "config.h"

// WiFi Fun sub-menu states
enum WiFiFunState {
  WIFI_FUN_MENU,
  FAKE_WIFI_INPUT,
  FAKE_WIFI_RUNNING,
  PORTALS_MENU,
  PORTAL_MANAGER_ACTIVE,
  PORTAL_INPUT,
  PORTAL_RUNNING_STATE,
  BIG_PARTY_MENU,
  RICKROLL_RUNNING,
  AP_LIST_EDIT,
  PARTY_TIME_EDIT,
  PARTY_TIME_RUNNING,
  PARTY_TIME_SAVE_SLOT,
  PARTY_TIME_LOAD_SLOT,
  TV_B_GONE_RUNNING,
  ANALYTICS_MENU,
  SPEEDOMETER_RUNNING,
  PROBE_SNIFFER_RUNNING,
  PORTAL_GAMES_RUNNING
};

// WiFi Fun functions
void enterWiFiFunApp();
void drawWiFiFunMenu();
void drawFakeWiFiInput();
void drawFakeWiFiRunning();
void drawPortalsMenu();
void drawPortalInput();
void drawBigPartyMenu();
void drawRickRoll();
void drawAPListEdit();
void startFakeWiFi(String ssid);
void stopFakeWiFi();
void startRickRoll();
void updateRickRoll();
void stopRickRoll();
void drawPartyTimeEdit();
void drawPartyTimeRunning();
void startPartyTime();
void updatePartyTime();
void stopPartyTime();
void handleWiFiFunNavigation(char key);

// Party Time SSID Set management
void savePartyTimeSet(int slot);
void loadPartyTimeSet(int slot);
void showPartyTimeSaveMenu();
void showPartyTimeLoadMenu();
void showPartyTimeManageMenu();

// Analytics functions
void drawAnalyticsMenu();
void drawCrowdCounter();
void drawSpeedometer();
void drawHeatmap();
void startCrowdCounter();
void updateCrowdCounter();
void stopCrowdCounter();
void startSpeedometer();
void updateSpeedometer();
void stopSpeedometer();
void startHeatmap();
void updateHeatmap();
void stopHeatmap();
void startProbeSniffer();
void updateProbeSniffer();
void stopProbeSniffer();
void drawProbeSniffer();

// WiFi Fun globals
extern WiFiFunState wifiFunState;
extern int wifiFunMenuIndex;
extern int portalsMenuIndex;
extern int bigPartyMenuIndex;
extern int analyticsMenuIndex;
extern String fakeSSID;
extern String portalSSIDInput;
extern int connectedClients;
extern bool rickRollActive;
extern bool partyTimeActive;
extern String partyTimeInput;
extern String apList[10];
extern int apListCount;
extern int partyTimeListScroll;

// Analytics globals
extern bool crowdCounterActive;
extern int deviceCount;
extern bool speedometerActive;
extern float currentSpeed;
extern float uploadSpeed;
extern float downloadSpeed;
extern bool speedTestRunning;
extern bool speedTestComplete;
extern bool heatmapActive;

// Portal Games functions
void startPortalGames();
void stopPortalGames();
void drawPortalGames();
void handlePortalGamesLoop();

// Portal Games globals
extern int portalGamesPlayers;
extern int portalGamesSubmissions;
extern int portalGamesVotes;

// TV-B-Gone (IR) functions
void startTVBGone();
void updateTVBGone();
void stopTVBGone();
void drawTVBGone();

// TV-B-Gone globals
extern bool tvbgoneActive;
extern int tvbgoneProgress;
extern int tvbgoneTotalCodes;

#endif