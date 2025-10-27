#include "esp_now_manager.h"

ESPNowManager espNowManager;

ESPNowManager::ESPNowManager() : initialized(false), peerCount(0),
                                  bytesSent(0), bytesReceived(0),
                                  messagesSent(0), messagesReceived(0),
                                  sendFailures(0) {
  memset(peers, 0, sizeof(peers));
}

void ESPNowManager::onDataRecv(const uint8_t* mac, const uint8_t* data, int len) {
  espNowManager.bytesReceived += len;
  espNowManager.messagesReceived++;

  // Update peer activity
  espNowManager.updatePeerActivity(mac);

  // Forward to message handler
  onMessageReceived(mac, data, len);
}

void ESPNowManager::onDataSent(const uint8_t* mac, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    espNowManager.sendFailures++;
  }
}

bool ESPNowManager::init(const uint8_t* pmk) {
  if (initialized) return true;

  // Set WiFi mode to STA (ESP-NOW requirement)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    return false;
  }

  // Set PMK for encryption
  if (esp_now_set_pmk(pmk) != ESP_OK) {
    esp_now_deinit();
    return false;
  }

  // Register callbacks
  esp_now_register_recv_cb(onDataRecv);
  esp_now_register_send_cb(onDataSent);

  // Add broadcast peer (unencrypted for discovery)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastMAC, 6);
  peerInfo.channel = ESP_NOW_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    esp_now_deinit();
    return false;
  }

  initialized = true;
  return true;
}

void ESPNowManager::deinit() {
  if (!initialized) return;

  esp_now_deinit();
  initialized = false;
  peerCount = 0;
  memset(peers, 0, sizeof(peers));
}

bool ESPNowManager::addPeer(const uint8_t* mac, const char* deviceID, const char* username) {
  if (peerCount >= MAX_PEERS) return false;

  // Check if peer already exists
  PeerDevice* existing = findPeer(mac);
  if (existing) {
    // Update existing peer
    strncpy(existing->deviceID, deviceID, 15);
    existing->deviceID[15] = '\0';
    strncpy(existing->username, username, 15);
    existing->username[15] = '\0';
    existing->lastSeen = millis();
    existing->active = true;
    return true;
  }

  // Add new peer to ESP-NOW
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, mac, 6);
  peerInfo.channel = ESP_NOW_CHANNEL;
  peerInfo.encrypt = true;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return false;
  }

  // Add to our peer list
  memcpy(peers[peerCount].mac, mac, 6);
  strncpy(peers[peerCount].deviceID, deviceID, 15);
  peers[peerCount].deviceID[15] = '\0';
  strncpy(peers[peerCount].username, username, 15);
  peers[peerCount].username[15] = '\0';
  peers[peerCount].lastSeen = millis();
  peers[peerCount].active = true;
  peerCount++;

  return true;
}

bool ESPNowManager::removePeer(const uint8_t* mac) {
  for (int i = 0; i < peerCount; i++) {
    if (memcmp(peers[i].mac, mac, 6) == 0) {
      // Remove from ESP-NOW
      esp_now_del_peer(mac);

      // Remove from our list (shift remaining)
      for (int j = i; j < peerCount - 1; j++) {
        peers[j] = peers[j + 1];
      }
      peerCount--;
      memset(&peers[peerCount], 0, sizeof(PeerDevice));
      return true;
    }
  }
  return false;
}

void ESPNowManager::updatePeerActivity(const uint8_t* mac) {
  PeerDevice* peer = findPeer(mac);
  if (peer) {
    peer->lastSeen = millis();
    peer->active = true;
  }
}

PeerDevice* ESPNowManager::findPeer(const uint8_t* mac) {
  for (int i = 0; i < peerCount; i++) {
    if (memcmp(peers[i].mac, mac, 6) == 0) {
      return &peers[i];
    }
  }
  return nullptr;
}

PeerDevice* ESPNowManager::findPeerByDeviceID(const char* deviceID) {
  for (int i = 0; i < peerCount; i++) {
    if (strcmp(peers[i].deviceID, deviceID) == 0) {
      return &peers[i];
    }
  }
  return nullptr;
}

PeerDevice* ESPNowManager::getPeer(int index) {
  if (index < 0 || index >= peerCount) return nullptr;
  return &peers[index];
}

void ESPNowManager::cleanupInactivePeers() {
  unsigned long now = millis();
  for (int i = peerCount - 1; i >= 0; i--) {
    if (now - peers[i].lastSeen > 300000) { // 5 minutes
      removePeer(peers[i].mac);
    }
  }
}

bool ESPNowManager::sendBroadcast(const uint8_t* data, size_t len) {
  if (!initialized) return false;

  esp_err_t result = esp_now_send(broadcastMAC, data, len);
  if (result == ESP_OK) {
    bytesSent += len;
    messagesSent++;
    return true;
  }
  sendFailures++;
  return false;
}

bool ESPNowManager::sendDirect(const uint8_t* mac, const uint8_t* data, size_t len) {
  if (!initialized) return false;

  esp_err_t result = esp_now_send(mac, data, len);
  if (result == ESP_OK) {
    bytesSent += len;
    messagesSent++;
    return true;
  }
  sendFailures++;
  return false;
}

bool ESPNowManager::sendToDeviceID(const char* deviceID, const uint8_t* data, size_t len) {
  PeerDevice* peer = findPeerByDeviceID(deviceID);
  if (!peer) return false;
  return sendDirect(peer->mac, data, len);
}
