#pragma once

#include "AudioStream.h"
#include <cmath>

class AudioAnalyzeFFT1024 : public AudioStream {
public:
    AudioAnalyzeFFT1024() : AudioStream(1, inputQueueArray), sample_count(0), new_data(false) {}
    
    bool available(void) { return new_data; }
    float read(unsigned int binNumber) {
        if (binNumber >= 512) return 0.0f;
        new_data = false;
        return output[binNumber];
    }
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    float buffer[1024] = {0.0f};
    float output[512] = {0.0f};
    uint16_t sample_count;
    volatile bool new_data;
};
