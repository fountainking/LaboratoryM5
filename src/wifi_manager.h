#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "config.h"

void scanWiFiNetworks();
void connectToWiFi();
void saveNetwork(String ssid, String password);
void deleteNetwork(int index);
void connectToSavedNetwork(int index);

#endif