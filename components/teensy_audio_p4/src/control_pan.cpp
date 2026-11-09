#include "control_pan.h"
#include <cmath>

void AudioEffectPan::pan(float p) {
    if (p < -1.0f) p = -1.0f;
    if (p > 1.0f) p = 1.0f;
    float angle = (p + 1.0f) * ((float)M_PI / 4.0f); // 0 to PI/2
    left_gain = (int32_t)(cosf(angle) * 32767.0f);
    right_gain = (int32_t)(sinf(angle) * 32767.0f);
}

void AudioEffectPan::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;

    audio_block_t *out_l = allocate();
    audio_block_t *out_r = allocate();

    if (!out_l || !out_r) {
        if (out_l) release(out_l);
        if (out_r) release(out_r);
        release(block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t s = block->data[i];
        out_l->data[i] = (int16_t)((s * left_gain) >> 15);
        out_r->data[i] = (int16_t)((s * right_gain) >> 15);
    }

    release(block);
    transmit(out_l, 0);
    transmit(out_r, 1);
    release(out_l);
    release(out_r);
}
