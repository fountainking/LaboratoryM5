#ifndef _AUDIOGENERATORMP3_PSRAM_H
#define _AUDIOGENERATORMP3_PSRAM_H

#include <AudioGeneratorMP3.h>
#include "psram_allocator.h"

// Wrapper class that uses PSRAM for MP3 decoding
class AudioGeneratorMP3_PSRAM : public AudioGeneratorMP3 {
public:
  AudioGeneratorMP3_PSRAM() : AudioGeneratorMP3() {
    Serial.println(F("AudioGeneratorMP3_PSRAM: Constructor"));
  }

  virtual ~AudioGeneratorMP3_PSRAM() {
    Serial.println(F("AudioGeneratorMP3_PSRAM: Destructor"));
  }

  // Override begin to show PSRAM usage
  virtual bool begin(AudioFileSource *source, AudioOutput *output) override {
    Serial.printf("Free PSRAM before MP3 begin: %d bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap before MP3 begin: %d bytes\n", ESP.getFreeHeap());

    bool result = AudioGeneratorMP3::begin(source, output);

    Serial.printf("Free PSRAM after MP3 begin: %d bytes\n", ESP.getFreePsram());
    Serial.printf("Free heap after MP3 begin: %d bytes\n", ESP.getFreeHeap());

    return result;
  }
};

#endif
