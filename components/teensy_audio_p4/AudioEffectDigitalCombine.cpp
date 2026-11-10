#include "AudioEffectDigitalCombine.h"

void AudioEffectDigitalCombine::update() {
    audio_block_t* blockA = receiveReadOnly(0);
    audio_block_t* blockB = receiveReadOnly(1);
    if (!blockA && !blockB) return;

    audio_block_t* out = allocate();
    if (!out) {
        if (blockA) release(blockA);
        if (blockB) release(blockB);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int16_t a = blockA ? blockA->data[i] : 0;
        int16_t b = blockB ? blockB->data[i] : 0;
        switch (m_mode) {
            case COMBINE_AND: out->data[i] = a & b; break;
            case COMBINE_OR:  out->data[i] = a | b; break;
            case COMBINE_XOR: out->data[i] = a ^ b; break;
            case COMBINE_MOD: out->data[i] = (b != 0) ? (a % b) : a; break;
        }
    }
    transmit(out, 0);
    release(out);
    if (blockA) release(blockA);
    if (blockB) release(blockB);
}
