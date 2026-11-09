#include "effect_midside.h"

static inline int16_t sat16(int32_t val) {
    if (val > 32767) return 32767;
    if (val < -32768) return -32768;
    return (int16_t)val;
}

void AudioEffectMidSide::update(void) {
    audio_block_t *in0 = receiveReadOnly(0);
    audio_block_t *in1 = receiveReadOnly(1);

    if (!in0 && !in1) return;

    audio_block_t *out0 = allocate();
    audio_block_t *out1 = allocate();

    if (!out0 || !out1) {
        if (out0) release(out0);
        if (out1) release(out1);
        if (in0) release(in0);
        if (in1) release(in1);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t s0 = in0 ? in0->data[i] : 0;
        int32_t s1 = in1 ? in1->data[i] : 0;
        if (!decode_mode) {
            // Encode: Mid = (L+R)/2, Side = (L-R)/2
            out0->data[i] = (int16_t)((s0 + s1) >> 1);
            out1->data[i] = (int16_t)((s0 - s1) >> 1);
        } else {
            // Decode: Left = Mid + Side, Right = Mid - Side
            out0->data[i] = sat16(s0 + s1);
            out1->data[i] = sat16(s0 - s1);
        }
    }

    if (in0) release(in0);
    if (in1) release(in1);
    transmit(out0, 0);
    transmit(out1, 1);
    release(out0);
    release(out1);
}
