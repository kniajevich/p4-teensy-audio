#pragma once

#include "AudioStream.h"

class AudioSynthNoiseWhite : public AudioStream {
public:
    AudioSynthNoiseWhite() : AudioStream(0, nullptr), pseudorandom_state(1) {}
    void amplitude(float n) {
        if (n < 0.0f) n = 0.0f;
        if (n > 1.0f) n = 1.0f;
        current_amplitude = (int16_t)(n * 32767.0f);
    }
    virtual void update(void) override;

private:
    uint32_t pseudorandom_state;
    int16_t current_amplitude = 32767;
};
