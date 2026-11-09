#include "filter_fir.h"

void AudioFilterFIR::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;
    if (!coeff || num_coeffs == 0) {
        release(block);
        return;
    }

    audio_block_t *out = allocate();
    if (!out) {
        release(block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        state[state_idx] = block->data[i];
        int32_t acc = 0;
        int idx = state_idx;
        for (int c = 0; c < num_coeffs; c++) {
            acc += ((int32_t)state[idx] * (int32_t)coeff[c]) >> 15;
            idx--;
            if (idx < 0) idx = 255;
        }
        if (acc > 32767) acc = 32767;
        if (acc < -32768) acc = -32768;
        out->data[i] = (int16_t)acc;
        state_idx = (state_idx + 1) & 255;
    }

    release(block);
    transmit(out, 0);
    release(out);
}
