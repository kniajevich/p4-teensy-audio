#pragma once

#include "AudioStream.h"
#include <string.h>

class AudioEffectFreeverb : public AudioStream {
public:
    AudioEffectFreeverb();
    void roomsize(float r);
    void damping(float d);
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    int16_t roomsize_val;
    int16_t damp_val;

    int16_t comb_buf[8][1700];
    int comb_len[8];
    int comb_idx[8];
    int16_t comb_filter[8];

    int16_t allpass_buf[4][600];
    int allpass_len[4];
    int allpass_idx[4];
};
