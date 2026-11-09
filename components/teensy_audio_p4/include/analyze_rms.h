#pragma once

#include "AudioStream.h"
#include <cmath>

class AudioAnalyzeRMS : public AudioStream {
public:
    AudioAnalyzeRMS() : AudioStream(1, inputQueueArray), sum(0), count(0), rms_val(0.0f) {}
    
    bool available(void) {
        return new_data;
    }
    
    float read(void) {
        new_data = false;
        return rms_val;
    }
    
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    volatile bool new_data = false;
    uint64_t sum;
    uint32_t count;
    float rms_val;
};
