#pragma once

#include "AudioStream.h"

class AudioSynthNoisePink : public AudioStream {
public:
    AudioSynthNoisePink() : AudioStream(0, nullptr) {}
    void amplitude(float n) {
        if (n < 0.0f) n = 0.0f;
        if (n > 1.0f) n = 1.0f;
        current_amplitude = (int16_t)(n * 32767.0f);
    }
    virtual void update(void) override;

private:
    int32_t b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0;
    uint32_t rand_seed = 22222;
    int16_t current_amplitude = 32767;
};
