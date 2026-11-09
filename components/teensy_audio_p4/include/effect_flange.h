#pragma once

#include "AudioStream.h"
#include <string.h>

class AudioEffectFlange : public AudioStream {
public:
    AudioEffectFlange() : AudioStream(1, inputQueueArray), delay_buf(nullptr), buf_len(0), offset_idx(64), depth_idx(32), lfo_inc(10000000u), lfo_phase(0), write_idx(0) {}
    bool begin(int16_t *delayline, int length, int offset, int depth, float rate);
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    int16_t *delay_buf;
    int buf_len;
    int offset_idx;
    int depth_idx;
    uint32_t lfo_inc;
    uint32_t lfo_phase;
    int write_idx;
};
