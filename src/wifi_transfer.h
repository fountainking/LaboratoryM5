#ifndef WIFI_TRANSFER_H
#define WIFI_TRANSFER_H

#include "config.h"
#include <WebServer.h>
#include <WiFi.h>
#include <SD.h>

// WiFi Transfer states
enum WiFiTransferState {
  TRANSFER_MENU,
  TRANSFER_RUNNING,
  TRANSFER_STOPPED
};

// WiFi Transfer functions
void enterWiFiTransferApp();
void drawTransferMenu();
void drawTransferRunning();
void drawTransferStopped();
void startWebServer();
void stopWebServer();
void handleWebServerLoop();
void handleFileUpload();
void handleFileList();
void handleDirectoryContents();  // NEW: Load directory on-demand
void handleFileDownload();
void handleFileDelete();
void handleRoot();

// WiFi Transfer globals
extern WiFiTransferState transferState;
extern WebServer* webServer;
extern bool serverRunning;
extern int uploadCount;
extern int downloadCount;
extern String lastAction;

#endif