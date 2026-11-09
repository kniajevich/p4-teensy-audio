#include "mixer4.h"

static inline int16_t saturate16(int32_t val) {
    if (val > 32767) return 32767;
    if (val < -32768) return -32768;
    return (int16_t)val;
}

void AudioMixer4::update(void) {
    audio_block_t *in[4];
    bool any_active = false;

    for (int i = 0; i < 4; i++) {
        in[i] = receiveReadOnly(i);
        if (in[i]) any_active = true;
    }

    if (!any_active) return;

    audio_block_t *out = allocate();
    if (!out) {
        for (int i = 0; i < 4; i++) if (in[i]) release(in[i]);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t sum = 0;
        for (int ch = 0; ch < 4; ch++) {
            if (in[ch]) {
                sum += ((int32_t)in[ch]->data[i] * multiplier[ch]) >> 16;
            }
        }
        out->data[i] = saturate16(sum);
    }

    for (int i = 0; i < 4; i++) {
        if (in[i]) release(in[i]);
    }

    transmit(out, 0);
    release(out);
}
