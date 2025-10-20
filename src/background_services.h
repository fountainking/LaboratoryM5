#ifndef BACKGROUND_SERVICES_H
#define BACKGROUND_SERVICES_H

#include "config.h"

// Service types
enum ServiceType {
  SERVICE_NONE = 0,
  SERVICE_FAKE_AP = 1,
  SERVICE_PORTAL = 2,
  SERVICE_TRANSFER = 4
};

// Service status structure
struct ServiceStatus {
  bool fakeAPRunning;
  bool portalRunning;
  bool transferRunning;
  String fakeAPName;
  String portalName;
  int portalVisitors;
  int connectedClients;
};

// Background service functions
void initBackgroundServices();
void startBackgroundFakeAP(const String& ssid);
void startBackgroundPortal(const String& ssid);
void startBackgroundTransfer();
void stopBackgroundFakeAP();
void stopBackgroundPortal();
void stopBackgroundTransfer();
void stopAllBackgroundServices();
ServiceStatus getServiceStatus();
bool isAnyServiceRunning();

// Task handles (for internal use)
extern TaskHandle_t portalTaskHandle;
extern TaskHandle_t transferTaskHandle;

#endif
