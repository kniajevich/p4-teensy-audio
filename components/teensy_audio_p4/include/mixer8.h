#pragma once

#include "AudioStream.h"

class AudioMixer8 : public AudioStream {
public:
    AudioMixer8() : AudioStream(8, inputQueueArray) {
        for (int i = 0; i < 8; i++) multiplier[i] = 65536; // 1.0 gain
    }
    
    void gain(unsigned int channel, float gain) {
        if (channel >= 8) return;
        if (gain < 0.0f) gain = 0.0f;
        if (gain > 32.0f) gain = 32.0f;
        multiplier[channel] = (int32_t)(gain * 65536.0f);
    }
    
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[8];
    int32_t multiplier[8];
};
