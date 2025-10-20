#ifndef PORTAL_MANAGER_H
#define PORTAL_MANAGER_H

#include "config.h"
#include <Preferences.h>

#define MAX_SAVED_PORTALS 10

// Portal profile structure
struct PortalProfile {
  String name;          // Display name for portal
  String ssid;          // WiFi SSID to broadcast
  String htmlPath;      // Path to HTML file on SD card (e.g., /portals/lab.html)
  bool useBuiltIn;      // If true, use built-in HTML instead of file
};

// Portal manager state
enum PortalManagerState {
  PM_LIST,
  PM_CREATE_NAME,
  PM_CREATE_SSID,
  PM_CREATE_FILE,
  PM_EDIT,
  PM_UPLOAD_HELP
};

// Portal manager functions
void enterPortalManager();
void loadSavedPortals();
void savePortal(const PortalProfile& portal);
void deletePortal(int index);
void drawPortalList();
void drawPortalCreate();
void drawPortalUploadHelp();
void handlePortalManagerNavigation(char key);
String loadPortalHTML(const PortalProfile& portal);

// Portal manager globals
extern PortalProfile savedPortals[MAX_SAVED_PORTALS];
extern int numSavedPortals;
extern int selectedPortalIndex;
extern PortalManagerState pmState;
extern PortalProfile currentPortal;
extern String portalInputBuffer;

#endif
