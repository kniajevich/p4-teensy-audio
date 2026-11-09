#include "synth_whitenoise.h"

void AudioSynthNoiseWhite::update(void) {
    audio_block_t *block = allocate();
    if (!block) return;

    if (current_amplitude == 0) {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            block->data[i] = 0;
        }
    } else {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            // 32-bit Xorshift PRNG
            pseudorandom_state ^= (pseudorandom_state << 13);
            pseudorandom_state ^= (pseudorandom_state >> 17);
            pseudorandom_state ^= (pseudorandom_state << 5);
            int16_t sample = (int16_t)(pseudorandom_state >> 16);
            block->data[i] = (int16_t)(((int32_t)sample * current_amplitude) >> 15);
        }
    }

    transmit(block, 0);
    release(block);
}
