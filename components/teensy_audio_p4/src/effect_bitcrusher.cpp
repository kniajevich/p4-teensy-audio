#include "effect_bitcrusher.h"

void AudioEffectBitcrusher::update(void) {
    audio_block_t *block = receiveWritable(0);
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        accumulator += 65536;
        if (accumulator >= sample_step) {
            accumulator -= sample_step;
            int16_t sample = block->data[i];
            if (shift_val > 0) {
                sample = (sample >> shift_val) << shift_val;
            }
            held_sample = sample;
        }
        block->data[i] = held_sample;
    }

    transmit(block, 0);
    release(block);
}
