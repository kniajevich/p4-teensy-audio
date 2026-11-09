#include "synth_waveform_mod.h"

void AudioSynthWaveformModulated::update(void) {
    audio_block_t *freq_mod_block = receiveReadOnly(0);
    audio_block_t *phase_mod_block = receiveReadOnly(1);

    audio_block_t *block = allocate();
    if (!block) {
        if (freq_mod_block) release(freq_mod_block);
        if (phase_mod_block) release(phase_mod_block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        uint32_t inc = phase_increment;
        if (freq_mod_block) {
            int32_t mod = ((int32_t)freq_mod_block->data[i] * modulation_factor) >> 15;
            inc = (uint32_t)((int64_t)inc * (32768 + mod) >> 15);
        }

        uint32_t ph = phase_accumulator;
        if (phase_mod_block) {
            ph += (uint32_t)phase_mod_block->data[i] << 16;
        }

        int16_t val = 0;
        switch (tone_type) {
            case WAVEFORM_SQUARE:
                val = (ph < 0x80000000) ? magnitude : -magnitude;
                break;
            case WAVEFORM_SAWTOOTH:
                val = (int16_t)(((int32_t)(ph >> 16) - 32768) * magnitude >> 15);
                break;
            case WAVEFORM_TRIANGLE: {
                if (ph < 0x80000000) {
                    val = -32768 + (int32_t)(ph >> 15);
                } else {
                    val = 32767 - (int32_t)((ph - 0x80000000) >> 15);
                }
                val = (int16_t)(((int32_t)val * magnitude) >> 15);
                break;
            }
            default:
                val = 0;
                break;
        }
        block->data[i] = val;
        phase_accumulator += inc;
    }

    if (freq_mod_block) release(freq_mod_block);
    if (phase_mod_block) release(phase_mod_block);
    transmit(block, 0);
    release(block);
}
