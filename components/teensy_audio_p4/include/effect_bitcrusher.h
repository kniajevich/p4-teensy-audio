#pragma once

#include "AudioStream.h"

class AudioEffectBitcrusher : public AudioStream {
public:
    AudioEffectBitcrusher() : AudioStream(1, inputQueueArray) {}
    
    void bits(uint8_t b) {
        if (b < 1) b = 1;
        if (b > 16) b = 16;
        bits_val = b;
        shift_val = 16 - b;
    }
    void sampleRate(float hz) {
        if (hz < 1.0f) hz = 1.0f;
        if (hz > AUDIO_SAMPLE_RATE_EXACT) hz = AUDIO_SAMPLE_RATE_EXACT;
        sample_step = (uint32_t)((AUDIO_SAMPLE_RATE_EXACT / hz) * 65536.0f);
    }
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    uint8_t bits_val = 16;
    uint8_t shift_val = 0;
    uint32_t sample_step = 65536;
    uint32_t accumulator = 0;
    int16_t held_sample = 0;
};
