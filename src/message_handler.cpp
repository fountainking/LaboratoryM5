#include "message_handler.h"
#include "security_manager.h"
#include "esp_now_manager.h"
#include <time.h>

MessageHandler messageHandler;

MessageHandler::MessageHandler() : queueHead(0), queueTail(0), queueCount(0), onMessageAddedCallback(nullptr) {
  memset(messageQueue, 0, sizeof(messageQueue));
}

bool MessageHandler::createMessage(MessageType type, const char* content,
                                    uint8_t channel, const char* targetID,
                                    SecureMessage* outMsg) {
  if (!securityManager.isConnected()) return false;

  memset(outMsg, 0, sizeof(SecureMessage));

  // Fill header
  outMsg->type = type;
  outMsg->version = PROTOCOL_VERSION;
  outMsg->timestamp = time(nullptr);

  // Device info
  strncpy(outMsg->deviceID, securityManager.getDeviceID(), 15);
  outMsg->deviceID[15] = '\0';

  // Username (load from preferences or use device ID)
  Preferences prefs;
  prefs.begin("labchat", true);
  String username = prefs.getString("username", securityManager.getDeviceID());
  prefs.end();
  strncpy(outMsg->username, username.c_str(), 15);
  outMsg->username[15] = '\0';

  // Channel and target
  outMsg->channel = channel;
  if (targetID) {
    strncpy(outMsg->targetID, targetID, 15);
    outMsg->targetID[15] = '\0';
  }

  // Payload
  size_t contentLen = strlen(content);
  if (contentLen > sizeof(outMsg->payload) - 1) {
    contentLen = sizeof(outMsg->payload) - 1;
  }
  outMsg->msgLen = contentLen;
  strncpy(outMsg->payload, content, contentLen);
  outMsg->payload[contentLen] = '\0';

  // Generate HMAC (over everything except HMAC field itself)
  size_t msgSize = sizeof(SecureMessage) - sizeof(outMsg->hmac);
  securityManager.generateHMAC((uint8_t*)outMsg, msgSize, outMsg->hmac);

  return true;
}

bool MessageHandler::verifyMessage(const SecureMessage* msg) {
  // Check version
  if (msg->version != PROTOCOL_VERSION) {
    Serial.printf("  Version mismatch: got %d, expected %d\n", msg->version, PROTOCOL_VERSION);
    return false;
  }

  // Verify HMAC
  size_t msgSize = sizeof(SecureMessage) - sizeof(msg->hmac);
  if (!securityManager.verifyHMAC((uint8_t*)msg, msgSize, msg->hmac)) {
    Serial.println("  HMAC verification failed!");
    return false;
  }

  // Check timestamp (reject messages more than 5 minutes old/future)
  uint32_t now = time(nullptr);
  int32_t timeDiff = (int32_t)(msg->timestamp - now);
  Serial.printf("  Timestamp check: msg=%u, now=%u, diff=%d\n", msg->timestamp, now, timeDiff);
  if (abs(timeDiff) > 300) {
    Serial.println("  Timestamp out of range (>300 seconds)!");
    return false;
  }

  return true;
}

bool MessageHandler::handleReceivedMessage(const uint8_t* mac, const uint8_t* data, int len) {
  Serial.printf("MessageHandler::handleReceivedMessage() - len=%d, expected=%d\n", len, sizeof(SecureMessage));

  if (len != sizeof(SecureMessage)) {
    Serial.println("  ERROR: Length mismatch!");
    return false;
  }

  const SecureMessage* msg = (const SecureMessage*)data;

  // Verify message
  Serial.println("  Verifying message...");
  if (!verifyMessage(msg)) {
    Serial.println("  ERROR: Message verification failed!");
    return false;
  }
  Serial.println("  Message verified successfully");

  // Ignore our own messages
  if (strcmp(msg->deviceID, securityManager.getDeviceID()) == 0) {
    Serial.println("  Ignoring own message");
    return true;
  }

  // Handle presence announcements
  if (msg->type == MSG_PRESENCE) {
    Serial.printf("  PRESENCE message from %s (%s)\n", msg->username, msg->deviceID);
    Serial.printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    espNowManager.addPeer(mac, msg->deviceID, msg->username);
    return true;
  }

  // Filter by message type
  if (msg->type == MSG_DIRECT) {
    // Check if message is for us
    if (strcmp(msg->targetID, securityManager.getDeviceID()) != 0) {
      return true; // Not for us, ignore silently
    }
  }

  // Add peer if not already known
  espNowManager.addPeer(mac, msg->deviceID, msg->username);

  // Create display message
  DisplayMessage displayMsg;
  strncpy(displayMsg.deviceID, msg->deviceID, 15);
  displayMsg.deviceID[15] = '\0';
  strncpy(displayMsg.username, msg->username, 15);
  displayMsg.username[15] = '\0';
  displayMsg.channel = msg->channel;
  displayMsg.type = (MessageType)msg->type;
  displayMsg.timestamp = msg->timestamp;
  displayMsg.content = String(msg->payload);
  displayMsg.isOwn = false;

  // Add to queue
  return addToQueue(displayMsg);
}

bool MessageHandler::addToQueue(const DisplayMessage& msg) {
  // Add to circular buffer
  messageQueue[queueHead] = msg;
  queueHead = (queueHead + 1) % MESSAGE_QUEUE_SIZE;

  if (queueCount < MESSAGE_QUEUE_SIZE) {
    queueCount++;
  } else {
    // Queue full, move tail forward
    queueTail = (queueTail + 1) % MESSAGE_QUEUE_SIZE;
  }

  // Trigger display update callback if set
  if (onMessageAddedCallback) {
    onMessageAddedCallback();
  }

  return true;
}

DisplayMessage* MessageHandler::getQueuedMessage(int index) {
  if (index < 0 || index >= queueCount) return nullptr;

  int actualIndex = (queueTail + index) % MESSAGE_QUEUE_SIZE;
  return &messageQueue[actualIndex];
}

void MessageHandler::clearQueue() {
  queueHead = 0;
  queueTail = 0;
  queueCount = 0;
  memset(messageQueue, 0, sizeof(messageQueue));
}

bool MessageHandler::sendBroadcast(const char* content, uint8_t channel) {
  Serial.printf("MessageHandler::sendBroadcast() - Content: \"%s\", Channel: %d\n", content, channel);

  SecureMessage msg;
  if (!createMessage(MSG_BROADCAST, content, channel, nullptr, &msg)) {
    Serial.println("  ERROR: createMessage failed!");
    return false;
  }

  Serial.printf("  Message created - Type: %d, DeviceID: %s, Username: %s\n",
                msg.type, msg.deviceID, msg.username);
  Serial.printf("  Payload: \"%s\", Length: %d\n", msg.payload, msg.msgLen);

  // Add to our own queue for display
  DisplayMessage displayMsg;
  strncpy(displayMsg.deviceID, msg.deviceID, 15);
  strncpy(displayMsg.username, msg.username, 15);
  displayMsg.channel = msg.channel;
  displayMsg.type = MSG_BROADCAST;
  displayMsg.timestamp = msg.timestamp;
  displayMsg.content = String(msg.payload);
  displayMsg.isOwn = true;
  addToQueue(displayMsg);

  // Send via ESP-NOW
  return espNowManager.sendBroadcast((uint8_t*)&msg, sizeof(msg));
}

bool MessageHandler::sendDirect(const char* targetID, const char* content) {
  SecureMessage msg;
  if (!createMessage(MSG_DIRECT, content, 0, targetID, &msg)) {
    return false;
  }

  // Add to our own queue
  DisplayMessage displayMsg;
  strncpy(displayMsg.deviceID, msg.deviceID, 15);
  strncpy(displayMsg.username, msg.username, 15);
  displayMsg.channel = 0;
  displayMsg.type = MSG_DIRECT;
  displayMsg.timestamp = msg.timestamp;
  displayMsg.content = String("→ ") + String(targetID) + ": " + String(msg.payload);
  displayMsg.isOwn = true;
  addToQueue(displayMsg);

  // Send to specific device
  return espNowManager.sendToDeviceID(targetID, (uint8_t*)&msg, sizeof(msg));
}

bool MessageHandler::sendPresence() {
  SecureMessage msg;
  if (!createMessage(MSG_PRESENCE, "online", 0, nullptr, &msg)) {
    return false;
  }

  return espNowManager.sendBroadcast((uint8_t*)&msg, sizeof(msg));
}

// ESP-NOW callback implementation
void onMessageReceived(const uint8_t* mac, const uint8_t* data, int len) {
  messageHandler.handleReceivedMessage(mac, data, len);
}
