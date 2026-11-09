#pragma once

#include "AudioStream.h"
#include <string.h>

class AudioEffectChorus : public AudioStream {
public:
    AudioEffectChorus() : AudioStream(1, inputQueueArray), delay_buf(nullptr), buf_len(0), num_voices(1), write_idx(0), use_golden_spread(false) {
        for (int i = 0; i < 4; i++) {
            lfo_phase[i] = (uint32_t)(i * 1073741824u);
            lfo_rate[i] = (uint32_t)(25000000u * (i + 1));
        }
    }
    bool begin(int16_t *delayline, int length, int voices);
    void voices(int v) { num_voices = (v < 1) ? 1 : ((v > 4) ? 4 : v); }
    void goldenSpread(bool enable = true);
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    int16_t *delay_buf;
    int buf_len;
    int num_voices;
    int write_idx;
    bool use_golden_spread;
    uint32_t lfo_phase[4];
    uint32_t lfo_rate[4];
};
