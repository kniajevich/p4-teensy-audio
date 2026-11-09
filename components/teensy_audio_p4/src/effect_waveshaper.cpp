#include "effect_waveshaper.h"

void AudioEffectWaveshaper::update(void) {
    audio_block_t *block = receiveWritable(0);
    if (!block) return;

    if (!waveshape_table || table_length < 2) {
        transmit(block, 0);
        release(block);
        return;
    }

    float scale = (float)(table_length - 1) / 65536.0f;
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t in_val = block->data[i] + 32768;
        float index_f = in_val * scale;
        int idx = (int)index_f;
        if (idx >= table_length - 1) {
            block->data[i] = (int16_t)(waveshape_table[table_length - 1] * 32767.0f);
        } else {
            float frac = index_f - idx;
            float v0 = waveshape_table[idx];
            float v1 = waveshape_table[idx + 1];
            float out_f = v0 + frac * (v1 - v0);
            block->data[i] = (int16_t)(out_f * 32767.0f);
        }
    }

    transmit(block, 0);
    release(block);
}
