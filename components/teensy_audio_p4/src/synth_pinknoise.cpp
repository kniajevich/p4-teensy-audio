#include "synth_pinknoise.h"

void AudioSynthNoisePink::update(void) {
    audio_block_t *block = allocate();
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        rand_seed = rand_seed * 1664525 + 1013904223;
        int32_t white = (int16_t)(rand_seed >> 16);
        
        // Paul Kellet's filter method
        b0 = 998855 * (b0 >> 20) + white * 583;
        b1 = 988761 * (b1 >> 20) + white * 1307;
        b2 = 942203 * (b2 >> 20) + white * 2374;
        b3 = 855883 * (b3 >> 20) + white * 4014;
        b4 = 649339 * (b4 >> 20) + white * 6498;
        b5 = 335116 * (b5 >> 20) + white * 8931;
        
        int32_t pink = (b0 + b1 + b2 + b3 + b4 + b5 + (white * 536)) >> 5;
        block->data[i] = (int16_t)(((int32_t)pink * current_amplitude) >> 15);
    }

    transmit(block, 0);
    release(block);
}
