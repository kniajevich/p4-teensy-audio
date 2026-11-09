#pragma once

#include "AudioStream.h"

class AudioMixer4 : public AudioStream {
public:
    AudioMixer4() : AudioStream(4, inputQueueArray) {
        for (int i = 0; i < 4; i++) multiplier[i] = 65536;
    }
    
    void gain(unsigned int channel, float gain) {
        if (channel >= 4) return;
        if (gain < 0.0f) gain = 0.0f;
        if (gain > 32.0f) gain = 32.0f;
        multiplier[channel] = (int32_t)(gain * 65536.0f);
    }
    
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[4];
    int32_t multiplier[4];
};
