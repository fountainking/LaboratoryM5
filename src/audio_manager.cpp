#include "audio_manager.h"
#include "config.h"
#include <SD.h>
#include <SPI.h>

// Shared audio hardware (ONE instance for entire system)
static AudioOutputM5Speaker *sharedAudioOutput = nullptr;
static int masterVolume = 50; // 0-100
static int savedVolumeBeforeMute = -1; // -1 means not muted

// Current audio source
static AudioSource currentSource = AUDIO_SOURCE_NONE;

// Music player state
static AudioGeneratorMP3 *musicMP3 = nullptr;
static AudioFileSourceSD *musicFile = nullptr;
static String currentMusicPath = "";

// Radio player state
static AudioGeneratorMP3 *radioMP3 = nullptr;
static AudioFileSourceHTTPStream *radioStream = nullptr;

void initAudioManager() {
  if (sharedAudioOutput == nullptr) {
    Serial.println("Audio Manager: Creating AudioOutputM5Speaker...");
    sharedAudioOutput = new AudioOutputM5Speaker();
    if (sharedAudioOutput) {
      Serial.println("Audio Manager: AudioOutputM5Speaker allocated");
      bool success = sharedAudioOutput->begin();
      Serial.printf("Audio Manager: AudioOutputM5Speaker->begin() returned %s\n", success ? "true" : "false");
      if (success) {
        // Set initial volume
        sharedAudioOutput->SetGain(masterVolume / 25.0); // 0-100 mapped to 0.0-4.0
        Serial.println("Audio Manager: AudioOutputM5Speaker initialized successfully");
      } else {
        Serial.println("Audio Manager: ERROR - AudioOutputM5Speaker begin() failed!");
        delete sharedAudioOutput;
        sharedAudioOutput = nullptr;
      }
    } else {
      Serial.println("Audio Manager: ERROR - Failed to allocate AudioOutputM5Speaker!");
    }
  } else {
    Serial.println("Audio Manager: AudioOutputM5Speaker already initialized");
  }
}

AudioOutputM5Speaker* getSharedAudioOutput() {
  if (sharedAudioOutput == nullptr) {
    initAudioManager();
  }
  return sharedAudioOutput;
}

AudioSource getCurrentAudioSource() {
  return currentSource;
}

// Volume control
void setMasterVolume(int vol) {
  if (vol < 0) vol = 0;
  if (vol > 100) vol = 100;
  masterVolume = vol;

  if (sharedAudioOutput) {
    sharedAudioOutput->SetGain(masterVolume / 25.0);
  }
}

int getMasterVolume() {
  return masterVolume;
}

void masterVolumeUp() {
  setMasterVolume(masterVolume + 5);
}

void masterVolumeDown() {
  setMasterVolume(masterVolume - 5);
}

// Temporary mute for navigation (avoids hearing skips during button presses)
void temporarilyMuteAudio() {
  if (savedVolumeBeforeMute == -1) {  // Only save if not already muted
    savedVolumeBeforeMute = masterVolume;
    if (sharedAudioOutput) {
      sharedAudioOutput->SetGain(0.0);
    }
  }
}

void restoreAudioVolume() {
  if (savedVolumeBeforeMute != -1) {  // Only restore if we have a saved volume
    if (sharedAudioOutput) {
      sharedAudioOutput->SetGain(savedVolumeBeforeMute / 25.0);
    }
    savedVolumeBeforeMute = -1;  // Clear saved volume
  }
}

// Music player implementation
bool startMusicPlayback(const String& path) {
  // Stop any existing audio first
  stopMusicPlayback();
  stopRadioStream();

  initAudioManager();

  // CRITICAL: Ensure SPI bus and SD card are initialized with correct pins
  // AudioFileSourceSD uses the global SD instance, so we need to make sure it's ready
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

  if (!SD.begin(SD_SPI_CS_PIN, SPI, SD_SPI_FREQ)) {
    Serial.println("ERROR: SD card not available for audio playback!");
    return false;
  }

  // Verify file exists before trying to play
  if (!SD.exists(path.c_str())) {
    Serial.printf("ERROR: Audio file not found: %s\n", path.c_str());
    return false;
  }

  Serial.printf("Starting playback: %s\n", path.c_str());
  Serial.printf("Free heap before MP3 creation: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Largest free block: %d bytes\n", ESP.getMaxAllocHeap());

  musicFile = new AudioFileSourceSD(path.c_str());
  if (!musicFile) {
    Serial.println("ERROR: Failed to create AudioFileSourceSD!");
    return false;
  }

  // Check if file was actually opened
  Serial.printf("AudioFileSourceSD created, checking if open...\n");
  if (!musicFile->isOpen()) {
    Serial.println("ERROR: AudioFileSourceSD failed to open file!");
    delete musicFile;
    musicFile = nullptr;
    return false;
  }
  Serial.printf("AudioFileSourceSD successfully opened, size: %d bytes\n", musicFile->getSize());

  musicMP3 = new AudioGeneratorMP3_PSRAM();
  if (!musicMP3) {
    Serial.println("ERROR: Failed to create AudioGeneratorMP3!");
    delete musicFile;
    musicFile = nullptr;
    return false;
  }

  Serial.println("Calling musicMP3->begin()...");
  Serial.printf("  Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("  musicFile = %p (open=%s, size=%d)\n", musicFile,
                musicFile->isOpen() ? "YES" : "NO", musicFile->getSize());
  Serial.printf("  sharedAudioOutput = %p\n", sharedAudioOutput);

  bool beginResult = musicMP3->begin(musicFile, sharedAudioOutput);
  Serial.printf("  Free heap after begin(): %d bytes\n", ESP.getFreeHeap());
  Serial.printf("  musicMP3->begin() returned: %s\n", beginResult ? "true" : "false");

  if (beginResult) {
    currentMusicPath = path;
    currentSource = AUDIO_SOURCE_MUSIC;
    Serial.println("SUCCESS: MP3 playback started!");
    Serial.println("Calling musicMP3->isRunning()...");
    bool running = musicMP3->isRunning();
    Serial.printf("  musicMP3->isRunning() = %s\n", running ? "true" : "false");
    return true;
  } else {
    Serial.println("ERROR: musicMP3->begin() failed!");
    Serial.println("  This usually means:");
    Serial.println("    1. Invalid MP3 file format");
    Serial.println("    2. File is corrupted");
    Serial.println("    3. Insufficient memory");
    delete musicMP3;
    delete musicFile;
    musicMP3 = nullptr;
    musicFile = nullptr;
    return false;
  }
}

void stopMusicPlayback() {
  if (musicMP3) {
    if (musicMP3->isRunning()) {
      musicMP3->stop();
    }
    delay(50);
    delete musicMP3;
    musicMP3 = nullptr;
  }

  if (musicFile) {
    delete musicFile;
    musicFile = nullptr;
  }

  if (currentSource == AUDIO_SOURCE_MUSIC) {
    currentSource = AUDIO_SOURCE_NONE;
  }

  currentMusicPath = "";
}

bool isMusicPlaying() {
  return (currentSource == AUDIO_SOURCE_MUSIC && musicMP3 && musicMP3->isRunning());
}

void updateMusicPlayback() {
  if (currentSource == AUDIO_SOURCE_MUSIC && musicMP3 && musicMP3->isRunning()) {
    if (!musicMP3->loop()) {
      // Track finished
      stopMusicPlayback();
    }
  }
}

// Radio player implementation
bool startRadioStream(const char* url) {
  // Stop any existing audio first
  stopMusicPlayback();
  stopRadioStream();

  delay(150); // Critical: ensure previous stream fully cleaned up

  initAudioManager();

  radioStream = new AudioFileSourceHTTPStream(url);
  if (!radioStream) {
    return false;
  }

  radioMP3 = new AudioGeneratorMP3_PSRAM();
  if (!radioMP3) {
    delete radioStream;
    radioStream = nullptr;
    return false;
  }

  if (radioMP3->begin(radioStream, sharedAudioOutput)) {
    currentSource = AUDIO_SOURCE_RADIO;
    return true;
  } else {
    delete radioMP3;
    delete radioStream;
    radioMP3 = nullptr;
    radioStream = nullptr;
    return false;
  }
}

void stopRadioStream() {
  if (radioMP3) {
    if (radioMP3->isRunning()) {
      radioMP3->stop();
    }
    delay(50);
    delete radioMP3;
    radioMP3 = nullptr;
  }

  if (radioStream) {
    radioStream->close();
    delay(100);
    delete radioStream;
    radioStream = nullptr;
  }

  if (currentSource == AUDIO_SOURCE_RADIO) {
    currentSource = AUDIO_SOURCE_NONE;
  }

  delay(100); // Extra delay to ensure resources released
}

bool isRadioStreaming() {
  return (currentSource == AUDIO_SOURCE_RADIO && radioMP3 && radioMP3->isRunning());
}

void updateRadioPlayback() {
  if (currentSource == AUDIO_SOURCE_RADIO && radioMP3 && radioMP3->isRunning()) {
    if (!radioMP3->loop()) {
      // Stream error
      stopRadioStream();
    }
  }
}
