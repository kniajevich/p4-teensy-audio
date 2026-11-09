#pragma once

#include "AudioStream.h"
#include <string.h>
#include <cmath>

class AudioAnalyzeNoteFreq : public AudioStream {
public:
    AudioAnalyzeNoteFreq() : AudioStream(1, inputQueueArray), sample_count(0), current_freq(0.0f), current_prob(0.0f), threshold(0.15f), new_data(false) {
        memset(buffer, 0, sizeof(buffer));
    }
    
    void begin(float thresh = 0.15f) { threshold = thresh; }
    bool available(void) { return new_data; }
    float read(void) { new_data = false; return current_freq; }
    float probability(void) const { return current_prob; }
    
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    float buffer[1024];
    uint16_t sample_count;
    float current_freq;
    float current_prob;
    float threshold;
    volatile bool new_data;
};
