#include "synth_waveform.h"
#include <cmath>

void AudioSynthWaveform::update(void) {
    audio_block_t *block = allocate();
    if (!block) return;

    if (magnitude == 0) {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) block->data[i] = 0;
        transmit(block, 0);
        release(block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int16_t val = 0;
        switch (tone_type) {
            case WAVEFORM_SQUARE:
                val = (phase_accumulator < 0x80000000) ? magnitude : -magnitude;
                break;
            case WAVEFORM_SAWTOOTH:
                val = (int16_t)(((int32_t)(phase_accumulator >> 16) - 32768) * magnitude >> 15);
                break;
            case WAVEFORM_TRIANGLE: {
                uint32_t p = phase_accumulator;
                if (p < 0x80000000) {
                    val = -32768 + (int32_t)(p >> 15);
                } else {
                    val = 32767 - (int32_t)((p - 0x80000000) >> 15);
                }
                val = (int16_t)(((int32_t)val * magnitude) >> 15);
                break;
            }
            case WAVEFORM_PULSE:
                val = (phase_accumulator < pulse_width) ? magnitude : -magnitude;
                break;
            default:
                val = 0;
                break;
        }
        block->data[i] = val;
        phase_accumulator += phase_increment;
    }

    transmit(block, 0);
    release(block);
}
