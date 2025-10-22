#include "audio_manager.h"
#include "config.h"
#include <SD.h>
#include <SPI.h>

// Shared audio hardware (ONE instance for entire system)
static AudioOutputI2S *sharedAudioOutput = nullptr;
static int masterVolume = 50; // 0-100 (default 50%)
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
  // Only initialize ONCE - never recreate the audio output object
  // This matches the minimal test program that works perfectly
  if (sharedAudioOutput == nullptr) {
    // CRITICAL: Stop M5Speaker to release I2S port 0 (only needed first time)
    M5Cardputer.Speaker.end();
    Serial.println("Audio Manager: Stopped M5Speaker to release I2S port 0");

    sharedAudioOutput = new AudioOutputI2S(0);  // Use I2S port 0 (now available)
    sharedAudioOutput->SetPinout(41, 43, 42); // M5Cardputer I2S pins
    sharedAudioOutput->SetOutputModeMono(true);
    sharedAudioOutput->SetGain(masterVolume / 25.0); // Set initial gain based on masterVolume (50/25 = 2.0)
    Serial.println("Audio Manager: Initialized AudioOutputI2S on port 0");
  }
}

AudioOutputI2S* getSharedAudioOutput() {
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
  Serial.printf("Starting music playback: %s\n", path.c_str());
  Serial.printf("Free heap before: %d bytes\n", ESP.getFreeHeap());

  // Stop any existing audio first
  stopMusicPlayback();
  stopRadioStream();

  initAudioManager();

  // DON'T reinitialize SD card here - it's already initialized by the main app
  // Calling SD.begin() multiple times causes conflicts and makes SD unavailable
  // Just verify the SD card is accessible and file exists
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

  // Create MP3 decoder fresh every time (prevents stale state issues)
  musicMP3 = new AudioGeneratorMP3();
  if (!musicMP3) {
    Serial.println("ERROR: Failed to create AudioGeneratorMP3!");
    delete musicFile;
    musicFile = nullptr;
    return false;
  }
  Serial.println("Created fresh MP3 decoder");

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
    // Don't delete musicMP3 - keep it alive for next song
    delete musicFile;
    musicFile = nullptr;
    return false;
  }
}

void stopMusicPlayback() {
  Serial.println("Stopping music playback...");

  if (musicMP3 && musicMP3->isRunning()) {
    Serial.println("Stopping MP3 decoder...");
    musicMP3->stop();
  }

  // CRITICAL FIX: Delete and recreate decoder to prevent stale state
  // This fixes the "second song causes freeze" issue
  if (musicMP3) {
    delete musicMP3;
    musicMP3 = nullptr;
    Serial.println("Deleted MP3 decoder - will recreate fresh on next play");
  }

  // Close file
  if (musicFile) {
    Serial.println("Closing music file...");
    delete musicFile;
    musicFile = nullptr;
    Serial.println("Music file closed");
  }

  if (currentSource == AUDIO_SOURCE_MUSIC) {
    currentSource = AUDIO_SOURCE_NONE;
    Serial.println("Audio Manager: Music stopped");
  }

  currentMusicPath = "";

  Serial.printf("Free heap after stop: %d bytes\n", ESP.getFreeHeap());
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

  radioMP3 = new AudioGeneratorMP3();
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

    // Don't restart M5Speaker here - it will be started on-demand by safeBeep() when needed
    // This prevents stealing I2S port 0 from subsequent audio playback
    Serial.println("Audio Manager: Radio stopped, I2S port 0 released");
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
