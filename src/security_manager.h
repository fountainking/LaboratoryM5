#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

// Security configuration
#define PIN_LENGTH 4
#define HMAC_KEY_SIZE 32
#define DEVICE_ID_SIZE 16
#define PMK_SIZE 16
#define NETWORK_NAME_SIZE 16

// Device-level PIN system (shared across apps)
class DevicePIN {
public:
  static bool isSet();
  static bool verify(String pin);
  static bool create(String pin);
  static bool change(String oldPin, String newPin);
  static void factoryReset(); // Emergency reset via SD card token
};

// Network security manager
class SecurityManager {
private:
  uint8_t hmacKey[HMAC_KEY_SIZE];
  uint8_t pmk[PMK_SIZE];
  char deviceID[DEVICE_ID_SIZE];
  char networkName[NETWORK_NAME_SIZE];
  bool initialized;

  void generateDeviceID();
  void deriveKeysFromPassword(String password);

public:
  SecurityManager();

  // Network setup
  bool createNetwork(String password, String name);
  bool joinNetwork(String password);
  void leaveNetwork();
  bool isConnected();

  // HMAC operations
  void generateHMAC(const uint8_t* data, size_t len, uint8_t* hmac);
  bool verifyHMAC(const uint8_t* data, size_t len, const uint8_t* hmac);

  // MAC address rotation (predictable from seed)
  void getRotatedMAC(uint8_t* mac, uint32_t timestamp);
  bool isValidRotatedMAC(const uint8_t* mac, uint32_t timestamp);

  // Getters
  const uint8_t* getPMK() { return pmk; }
  const char* getDeviceID() { return deviceID; }
  const char* getNetworkName() { return networkName; }

  // Storage
  void saveToPreferences();
  bool loadFromPreferences();
  void clearPreferences();
};

extern SecurityManager securityManager;

#endif
