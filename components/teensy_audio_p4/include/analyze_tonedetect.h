#pragma once

#include "AudioStream.h"

class AudioAnalyzeToneDetect : public AudioStream {
public:
    AudioAnalyzeToneDetect() : AudioStream(1, inputQueueArray), coeff(0), s_prev(0.0f), s_prev2(0.0f), magnitude_val(0.0f), sample_idx(0), new_data(false) {}
    void set_frequency(float freq, int cycles = 128);
    bool available(void) { return new_data; }
    float read(void) { new_data = false; return magnitude_val; }
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    float coeff;
    float s_prev;
    float s_prev2;
    float magnitude_val;
    int sample_idx;
    volatile bool new_data;
};
