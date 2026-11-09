#pragma once

#include "AudioStream.h"
#include <string.h>

class AudioEffectChorus : public AudioStream {
public:
    AudioEffectChorus() : AudioStream(1, inputQueueArray), delay_buf(nullptr), buf_len(0), num_voices(1), write_idx(0) {
        for (int i = 0; i < 4; i++) lfo_phase[i] = (uint32_t)(i * 1073741824u);
    }
    bool begin(int16_t *delayline, int length, int voices);
    void voices(int v) { num_voices = (v < 1) ? 1 : ((v > 4) ? 4 : v); }
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    int16_t *delay_buf;
    int buf_len;
    int num_voices;
    int write_idx;
    uint32_t lfo_phase[4];
};
