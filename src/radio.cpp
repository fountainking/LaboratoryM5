#include "radio.h"
#include "ui.h"
#include "settings.h"
#include "audio_manager.h"

// Global radio state
RadioState radioState = RADIO_STOPPED;
int currentStationIndex = 0;
int volume = 50; // 0-100

// Built-in station list (verified working streams - HTTP + MP3 only)
const RadioStation stations[] = {
  {"KEXP 90.3 Seattle", "http://kexp-mp3-128.streamguys1.com/kexp128.mp3", "Alternative", 128},
  {"Radio Paradise", "http://stream.radioparadise.com/mp3-128", "Eclectic", 128},
  {"SomaFM Groove Salad", "http://ice1.somafm.com/groovesalad-128-mp3", "Ambient", 128},
  {"SomaFM Drone Zone", "http://ice1.somafm.com/dronezone-128-mp3", "Ambient", 128},
  {"SomaFM Beat Blender", "http://ice1.somafm.com/beatblender-128-mp3", "Electronic", 128},
  {"SomaFM DEF CON Radio", "http://ice1.somafm.com/defcon-128-mp3", "Hacker", 128},
  {"SomaFM Metal", "http://ice1.somafm.com/metal-128-mp3", "Metal", 128},
  {"WFMU Jersey City", "http://stream0.wfmu.org/freeform-128k", "Freeform", 128},
  {"SomaFM Secret Agent", "http://ice1.somafm.com/secretagent-128-mp3", "Lounge", 128},
  {"SomaFM Folk Forward", "http://ice1.somafm.com/folkfwd-128-mp3", "Folk", 128},
  {"SomaFM Lush", "http://ice1.somafm.com/lush-128-mp3", "Chillout", 128},
  {"SomaFM Indie Pop", "http://ice1.somafm.com/indiepop-128-mp3", "Indie", 128},
  {"SomaFM Space Station", "http://ice1.somafm.com/spacestation-128-mp3", "Ambient", 128},
  {"SomaFM Suburbs", "http://ice1.somafm.com/suburbsofgoa-128-mp3", "Psychedelic", 128},
  {"SomaFM Seven Inch Soul", "http://ice1.somafm.com/7soul-128-mp3", "Soul/Funk", 128},
  {"SomaFM Illinois Street", "http://ice1.somafm.com/illstreet-128-mp3", "Hip-Hop", 128},
  {"SomaFM Cliq Hop", "http://ice1.somafm.com/cliqhop-128-mp3", "IDM", 128},
  {"SomaFM Sonic Universe", "http://ice1.somafm.com/sonicuniverse-128-mp3", "Jazz/Soul", 128},
  {"SomaFM Dub Step", "http://ice1.somafm.com/dubstep-128-mp3", "Dubstep", 128},
  {"SomaFM Underground 80s", "http://ice1.somafm.com/u80s-128-mp3", "80s", 128},
  {"SomaFM Fluid", "http://ice1.somafm.com/fluid-128-mp3", "Downtempo", 128},
  {"SomaFM Deep Space One", "http://ice1.somafm.com/deepspaceone-128-mp3", "Deep Ambient", 128},
  {"SomaFM Mission Control", "http://ice1.somafm.com/missioncontrol-128-mp3", "Space Music", 128},
  {"SomaFM Digitalis", "http://ice1.somafm.com/digitalis-128-mp3", "Downtempo", 128},
  {"SomaFM Left Coast 70s", "http://ice1.somafm.com/leftcoast70s-128-mp3", "70s Rock", 128},
  {"SomaFM Boot Liquor", "http://ice1.somafm.com/bootliquor-128-mp3", "Americana", 128},
  {"SomaFM ThistleRadio", "http://ice1.somafm.com/thistle-128-mp3", "Celtic", 128},
  {"SomaFM Doomed", "http://ice1.somafm.com/doomed-128-mp3", "Dark Ambient", 128},
  {"SomaFM BAGeL Radio", "http://ice1.somafm.com/bagel-128-mp3", "Jewish", 128},
  {"SomaFM PopTron", "http://ice1.somafm.com/poptron-128-mp3", "Electropop", 128}
};

const int stationCount = 30;

void enterRadioApp() {
  initAudioManager();
  // Sync local volume with master volume
  volume = getMasterVolume();
  radioState = RADIO_STOPPED;
  drawRadioUI();
}

void exitRadioApp() {
  // Keep radio playing in background when exiting app
  // Don't call stopRadio() anymore
}

void drawRadioUI() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  drawStatusBar(false);

  // No "RADIO" title - removed

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(0xF81F);  // Bright purple for station name

  String stationName = String(stations[currentStationIndex].name);
  if (stationName.length() > 35) {
    stationName = stationName.substring(0, 35) + "...";
  }
  M5Cardputer.Display.drawString(stationName.c_str(), 10, 35);  // Lifted up

  M5Cardputer.Display.setTextColor(0xC99F);  // Light purple for genre
  String genre = String(stations[currentStationIndex].genre);
  M5Cardputer.Display.drawString(genre.c_str(), 10, 50);  // Lifted

  M5Cardputer.Display.setTextColor(0x780F);  // Deep purple for bitrate
  String bitrate = String(stations[currentStationIndex].bitrate) + " kbps";
  M5Cardputer.Display.drawString(bitrate.c_str(), 10, 62);  // Lifted

  M5Cardputer.Display.setTextSize(1);
  switch (radioState) {
    case RADIO_STOPPED:
      M5Cardputer.Display.setTextColor(0x780F);  // Deep purple
      M5Cardputer.Display.drawString("Stopped", 90, 75);  // Lifted
      break;
    case RADIO_CONNECTING:
      M5Cardputer.Display.setTextColor(0xC99F);  // Light purple
      M5Cardputer.Display.drawString("Connecting...", 80, 75);
      break;
    case RADIO_PLAYING:
      M5Cardputer.Display.setTextColor(0xF81F);  // Bright purple
      M5Cardputer.Display.drawString("Now Playing", 80, 75);
      break;
    case RADIO_ERROR:
      M5Cardputer.Display.setTextColor(TFT_RED);  // Keep red for errors
      M5Cardputer.Display.drawString("Connection Failed", 70, 75);
      break;
  }

  M5Cardputer.Display.setTextColor(0xA11F);  // Medium purple for "Volume:"
  M5Cardputer.Display.drawString("Volume:", 10, 95);  // Lifted
  M5Cardputer.Display.drawRect(55, 95, 102, 8, 0x780F);  // Deep purple border
  int volumeWidth = (volume * 100) / 100;
  M5Cardputer.Display.fillRect(56, 96, volumeWidth, 6, 0xC99F);  // Light purple fill
  M5Cardputer.Display.drawString(String(volume).c_str(), 165, 95);

  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(0x780F);  // Deep purple for instructions
  M5Cardputer.Display.drawString(";/. =Stations  Space=Play/Stop", 15, 110);  // Lifted
  M5Cardputer.Display.drawString("+/- =Volume  `=Back", 30, 120);
}

void playStation(int stationIndex) {
  stopRadio();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Radio: WiFi not connected!");
    radioState = RADIO_ERROR;
    drawRadioUI();
    return;
  }

  currentStationIndex = stationIndex;
  radioState = RADIO_CONNECTING;
  drawRadioUI();

  Serial.printf("Radio: Connecting to %s\n", stations[stationIndex].name);

  if (startRadioStream(stations[stationIndex].url)) {
    Serial.println("Radio: Playback started");
    radioState = RADIO_PLAYING;
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(1200, 50);
  } else {
    Serial.println("Radio: Failed to begin playback");
    radioState = RADIO_ERROR;
  }

  drawRadioUI();
}

void stopRadio() {
  stopRadioStream();
  radioState = RADIO_STOPPED;
}

void nextStation() {
  currentStationIndex = (currentStationIndex + 1) % stationCount;
  if (radioState == RADIO_PLAYING || radioState == RADIO_CONNECTING) {
    playStation(currentStationIndex);
  } else {
    drawRadioUI();
  }
}

void previousStation() {
  currentStationIndex = (currentStationIndex - 1 + stationCount) % stationCount;
  if (radioState == RADIO_PLAYING || radioState == RADIO_CONNECTING) {
    playStation(currentStationIndex);
  } else {
    drawRadioUI();
  }
}

void increaseVolume() {
  if (volume < 100) {
    volume += 10;
    if (volume > 100) volume = 100;
    setMasterVolume(volume);
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(1000, 30);

    M5Cardputer.Display.fillRect(55, 110, 102, 8, TFT_BLACK);
    M5Cardputer.Display.drawRect(55, 110, 102, 8, TFT_DARKGREY);
    int volumeWidth = (volume * 100) / 100;
    M5Cardputer.Display.fillRect(56, 111, volumeWidth, 6, TFT_CYAN);
    M5Cardputer.Display.fillRect(165, 110, 15, 8, TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString(String(volume).c_str(), 165, 110);
  }
}

void decreaseVolume() {
  if (volume > 0) {
    volume -= 10;
    if (volume < 0) volume = 0;
    setMasterVolume(volume);
    if (settings.soundEnabled) M5Cardputer.Speaker.tone(800, 30);

    M5Cardputer.Display.fillRect(55, 110, 102, 8, TFT_BLACK);
    M5Cardputer.Display.drawRect(55, 110, 102, 8, TFT_DARKGREY);
    int volumeWidth = (volume * 100) / 100;
    M5Cardputer.Display.fillRect(56, 111, volumeWidth, 6, TFT_CYAN);
    M5Cardputer.Display.fillRect(165, 110, 15, 8, TFT_BLACK);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString(String(volume).c_str(), 165, 110);
  }
}

void handleRadioNavigation(char key) {
  if (key == ';' || key == ',') {
    previousStation();
  } else if (key == '.' || key == '/') {
    nextStation();
  } else if (key == ' ') {
    if (radioState == RADIO_PLAYING) {
      stopRadio();
      drawRadioUI();
    } else if (radioState == RADIO_STOPPED || radioState == RADIO_ERROR) {
      playStation(currentStationIndex);
    }
  } else if (key == '+' || key == '=') {
    increaseVolume();
  } else if (key == '-' || key == '_') {
    decreaseVolume();
  }
}

void updateRadioApp() {
  if (radioState == RADIO_PLAYING) {
    // The audio manager handles the actual playback update
    if (!isRadioStreaming()) {
      radioState = RADIO_ERROR;
      drawRadioUI();
    }
  }
}

bool isRadioPlaying() {
  return (radioState == RADIO_PLAYING && isRadioStreaming());
}
