#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <Arduino.h>

// Message types
enum MessageType : uint8_t {
  MSG_BROADCAST = 0x01,
  MSG_DIRECT = 0x02,
  MSG_CHANNEL = 0x03,
  MSG_SYSTEM = 0x04,
  MSG_PRESENCE = 0x05
};

// Protocol version
#define PROTOCOL_VERSION 1

// Message structure (250 bytes total - ESP-NOW v1.0 limit)
struct SecureMessage {
  uint8_t type;           // 1 byte: message type
  uint8_t version;        // 1 byte: protocol version
  uint32_t timestamp;     // 4 bytes: unix timestamp
  char deviceID[16];      // 16 bytes: sender device ID
  char username[16];      // 16 bytes: sender username
  uint8_t channel;        // 1 byte: channel number (0-9)
  char targetID[16];      // 16 bytes: target device ID (for DM)
  uint16_t msgLen;        // 2 bytes: payload length
  char payload[165];      // 165 bytes: actual message content
  uint8_t hmac[32];       // 32 bytes: HMAC-SHA256
  // Total: 250 bytes
} __attribute__((packed));

// Display message (for UI)
struct DisplayMessage {
  char deviceID[16];
  char username[16];
  uint8_t channel;
  MessageType type;
  unsigned long timestamp;
  String content;
  bool isOwn;
};

// Message queue for display
#define MESSAGE_QUEUE_SIZE 50

class MessageHandler {
private:
  DisplayMessage messageQueue[MESSAGE_QUEUE_SIZE];
  int queueHead;
  int queueTail;
  int queueCount;

  bool verifyMessage(const SecureMessage* msg);

  // Callback for display updates
  void (*onMessageAddedCallback)();

public:
  MessageHandler();

  // Message creation
  bool createMessage(MessageType type, const char* content, uint8_t channel,
                     const char* targetID, SecureMessage* outMsg);

  // Message handling (called by ESP-NOW callback)
  bool handleReceivedMessage(const uint8_t* mac, const uint8_t* data, int len);

  // Queue management
  bool addToQueue(const DisplayMessage& msg);
  DisplayMessage* getQueuedMessage(int index);
  int getQueueCount() { return queueCount; }
  void clearQueue();

  // Sending helpers
  bool sendBroadcast(const char* content, uint8_t channel = 0);
  bool sendDirect(const char* targetID, const char* content);
  bool sendPresence(); // Announce presence to network

  // Set callback for real-time display updates
  void setMessageCallback(void (*callback)()) { onMessageAddedCallback = callback; }
};

extern MessageHandler messageHandler;

// ESP-NOW callback implementation
void onMessageReceived(const uint8_t* mac, const uint8_t* data, int len);

#endif
