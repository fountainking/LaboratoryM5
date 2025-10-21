#ifndef _AUDIOOUTPUTM5SPEAKER_H
#define _AUDIOOUTPUTM5SPEAKER_H

#include <M5Cardputer.h>
#include <AudioOutput.h>

// Ultra-simple AudioOutput class for M5Cardputer
// Minimal implementation - just output samples directly
class AudioOutputM5Speaker : public AudioOutput
{
  public:
    AudioOutputM5Speaker() {
      Serial.println("AudioOutputM5Speaker: Simple constructor");
    }

    virtual ~AudioOutputM5Speaker() {
      Serial.println("AudioOutputM5Speaker: Destructor");
    }

    virtual bool begin() override {
      Serial.println("AudioOutputM5Speaker: begin()");
      M5Cardputer.Speaker.setVolume(255);  // Max volume
      return true;
    }

    virtual bool ConsumeSample(int16_t sample[2]) override {
      // Accumulate samples in buffer
      if (bufferPos < BUFFER_SIZE) {
        buffer[bufferPos++] = sample[0];  // Left
        buffer[bufferPos++] = sample[1];  // Right
        return true;
      }

      // Buffer full - play it
      flush();
      return false;
    }

    virtual void flush() override {
      if (bufferPos > 0) {
        // Play buffer: playRaw(data, length, sampleRate, stereo, repeat, channel)
        M5Cardputer.Speaker.playRaw(buffer, bufferPos, hertz, true, 1, 0);
        bufferPos = 0;
      }
    }


    virtual bool stop() override {
      Serial.println("AudioOutputM5Speaker: stop()");
      M5Cardputer.Speaker.stop();
      return true;
    }

    bool SetRate(int hz) override {
      Serial.printf("AudioOutputM5Speaker: SetRate(%d)\n", hz);
      hertz = hz;
      return true;
    }

    bool SetBitsPerSample(int bits) override {
      return (bits == 16);
    }

    bool SetChannels(int channels) override {
      return (channels >= 1 && channels <= 2);
    }

    bool SetGain(float gain) override {
      int vol = (int)(gain * 64.0);
      if (vol > 255) vol = 255;
      if (vol < 0) vol = 0;
      M5Cardputer.Speaker.setVolume(vol);
      return true;
    }

  private:
    static const int BUFFER_SIZE = 256;
    int16_t buffer[BUFFER_SIZE];
    int bufferPos = 0;
    int hertz = 44100;
};

#endif
