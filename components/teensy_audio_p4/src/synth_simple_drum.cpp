#include "synth_simple_drum.h"
#include <cmath>

void AudioSynthSimpleDrum::length(int ms) {
    if (ms < 5) ms = 5;
    float total_samples = (float)ms * (AUDIO_SAMPLE_RATE_EXACT / 1000.0f);
    decay_rate = powf(0.001f, 1.0f / total_samples);
}

void AudioSynthSimpleDrum::noteOn(void) {
    cur_freq = base_freq * (1.0f + pitch_depth);
    amp = 1.0f;
    phase = 0;
    active = true;
}

void AudioSynthSimpleDrum::update(void) {
    if (!active) return;
    audio_block_t *block = allocate();
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        uint32_t inc = (uint32_t)(cur_freq * (4294967296.0f / AUDIO_SAMPLE_RATE_EXACT));
        phase += inc;
        float sin_val = sinf((float)phase * (2.0f * (float)M_PI / 4294967296.0f));

        seed = seed * 1664525u + 1013904223u;
        float noise_val = (float)((int16_t)(seed >> 16)) / 32768.0f;

        float sample = (sin_val * (1.0f - noise_mix) + noise_val * noise_mix) * amp;
        block->data[i] = (int16_t)(sample * 32767.0f);

        amp *= decay_rate;
        cur_freq += (base_freq - cur_freq) * (1.0f - decay_rate) * 2.0f;

        if (amp < 0.0001f) {
            active = false;
            for (int j = i + 1; j < AUDIO_BLOCK_SAMPLES; j++) block->data[j] = 0;
            break;
        }
    }

    transmit(block, 0);
    release(block);
}
