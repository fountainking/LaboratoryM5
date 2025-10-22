#ifndef _AUDIOOUTPUTM5SPEAKER_H
#define _AUDIOOUTPUTM5SPEAKER_H

#include <M5Cardputer.h>
#include <AudioOutputI2S.h>

// Simple wrapper that uses standard I2S output for M5Cardputer
// M5Cardputer Speaker uses I2S internally
class AudioOutputM5Speaker : public AudioOutputI2S
{
  public:
    AudioOutputM5Speaker() : AudioOutputI2S() {
      Serial.println("AudioOutputM5Speaker: Using I2S");
    }

    virtual ~AudioOutputM5Speaker() {
      Serial.println("AudioOutputM5Speaker: Destructor");
    }

    virtual bool begin() override {
      Serial.println("AudioOutputM5Speaker: Initializing I2S for M5Cardputer");

      // M5Cardputer speaker I2S pins
      SetPinout(41, 43, 42);  // BCLK, WCLK (LRC), DOUT

      // Start I2S with standard settings
      bool result = AudioOutputI2S::begin();

      if (result) {
        Serial.println("AudioOutputM5Speaker: I2S initialized successfully");
        SetGain(2.0);  // Set moderate gain
      } else {
        Serial.println("AudioOutputM5Speaker: I2S initialization failed");
      }

      return result;
    }

    virtual bool stop() override {
      Serial.println("AudioOutputM5Speaker: stop()");
      return AudioOutputI2S::stop();
    }
};

#endif
