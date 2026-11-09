#pragma once

#include "AudioStream.h"

class AudioEffectWaveshaper : public AudioStream {
public:
    AudioEffectWaveshaper() : AudioStream(1, inputQueueArray), waveshape_table(nullptr), table_length(0) {}
    
    void shape(const float *waveshape, int length) {
        waveshape_table = waveshape;
        table_length = length;
    }
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    const float *waveshape_table;
    int table_length;
};
