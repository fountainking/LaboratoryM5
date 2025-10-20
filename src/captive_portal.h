#ifndef CAPTIVE_PORTAL_H
#define CAPTIVE_PORTAL_H

#include "config.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// Portal state
enum PortalState {
  PORTAL_STOPPED,
  PORTAL_RUNNING
};

// Forward declaration
struct PortalProfile;

// Portal functions
void startCaptivePortal(const String& ssid);
void startCaptivePortalFromProfile(const PortalProfile& profile);
void stopCaptivePortal();
void handlePortalLoop();
bool isPortalRunning();
void drawPortalScreen();

// Portal globals
extern PortalState portalState;
extern WebServer* portalWebServer;
extern DNSServer* portalDNS;
extern int portalVisitorCount;
extern String portalSSID;

#endif
