#include "control_amp.h"

static inline int16_t saturate16(int32_t val) {
    if (val > 32767) return 32767;
    if (val < -32768) return -32768;
    return (int16_t)val;
}

void AudioAmplifier::update(void) {
    audio_block_t *block = receiveWritable(0);
    if (!block) return;

    if (gain_factor == 0) {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) block->data[i] = 0;
    } else if (gain_factor != 32768) {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            int32_t sample = ((int32_t)block->data[i] * gain_factor) >> 15;
            block->data[i] = saturate16(sample);
        }
    }

    transmit(block, 0);
    release(block);
}
