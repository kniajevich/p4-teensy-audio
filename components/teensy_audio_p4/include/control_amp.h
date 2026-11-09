#pragma once

#include "AudioStream.h"

class AudioAmplifier : public AudioStream {
public:
    AudioAmplifier() : AudioStream(1, inputQueueArray), gain_factor(32768) {}
    
    void gain(float n) {
        if (n < 0.0f) n = 0.0f;
        if (n > 32.0f) n = 32.0f;
        gain_factor = (int32_t)(n * 32768.0f);
    }
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    int32_t gain_factor;
};
