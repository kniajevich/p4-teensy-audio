#pragma once

#include "AudioStream.h"
#include <string.h>

class AudioSynthKarplusStrong : public AudioStream {
public:
    AudioSynthKarplusStrong() : AudioStream(0, nullptr), buffer_len(0), index(0), magnitude(0), seed(12345) {
        memset(buffer, 0, sizeof(buffer));
    }
    void noteOn(float freq, float velocity);
    void noteOff(float velocity);
    virtual void update(void) override;
private:
    int16_t buffer[1024];
    int buffer_len;
    int index;
    int16_t magnitude;
    uint32_t seed;
};
