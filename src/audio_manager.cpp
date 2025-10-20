#include "audio_manager.h"

// Shared audio hardware (ONE instance for entire system)
static AudioOutputI2S *sharedAudioOutput = nullptr;
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
    sharedAudioOutput = new AudioOutputI2S();
    sharedAudioOutput->SetPinout(41, 43, 42); // M5Cardputer I2S pins
    sharedAudioOutput->SetOutputModeMono(true);
    sharedAudioOutput->SetGain(masterVolume / 25.0); // 0-100 mapped to 0.0-4.0
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
  // Stop any existing audio first
  stopMusicPlayback();
  stopRadioStream();

  initAudioManager();

  musicFile = new AudioFileSourceSD(path.c_str());
  if (!musicFile) {
    return false;
  }

  musicMP3 = new AudioGeneratorMP3();
  if (!musicMP3) {
    delete musicFile;
    musicFile = nullptr;
    return false;
  }

  if (musicMP3->begin(musicFile, sharedAudioOutput)) {
    currentMusicPath = path;
    currentSource = AUDIO_SOURCE_MUSIC;
    return true;
  } else {
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
