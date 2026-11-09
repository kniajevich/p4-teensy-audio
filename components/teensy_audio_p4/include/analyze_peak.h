#pragma once

#include "AudioStream.h"

class AudioAnalyzePeak : public AudioStream {
public:
    AudioAnalyzePeak() : AudioStream(1, inputQueueArray), max_sample(0), min_sample(0) {}
    
    bool available(void) {
        return new_data;
    }
    
    float read(void) {
        new_data = false;
        int16_t m = max_sample;
        int16_t n = min_sample;
        max_sample = 0;
        min_sample = 0;
        if (n < 0) n = -n;
        if (n > m) m = n;
        return (float)m / 32767.0f;
    }
    
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    volatile bool new_data = false;
    int16_t max_sample;
    int16_t min_sample;
};
