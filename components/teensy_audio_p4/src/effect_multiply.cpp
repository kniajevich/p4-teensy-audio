#include "effect_multiply.h"

void AudioEffectMultiply::update(void) {
    audio_block_t *in0 = receiveReadOnly(0);
    audio_block_t *in1 = receiveReadOnly(1);

    if (!in0 && !in1) return;

    if (!in0 || !in1) {
        if (in0) release(in0);
        if (in1) release(in1);
        return;
    }

    audio_block_t *out = allocate();
    if (!out) {
        release(in0);
        release(in1);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // Signed 16-bit Q15 multiplication
        int32_t prod = ((int32_t)in0->data[i] * (int32_t)in1->data[i]) >> 15;
        if (prod > 32767) prod = 32767;
        else if (prod < -32768) prod = -32768;
        out->data[i] = (int16_t)prod;
    }

    release(in0);
    release(in1);
    transmit(out, 0);
    release(out);
}
