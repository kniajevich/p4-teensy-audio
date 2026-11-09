#include "analyze_tonedetect.h"
#include <cmath>

void AudioAnalyzeToneDetect::set_frequency(float freq, int cycles) {
    (void)cycles;
    if (freq <= 0.0f) freq = 1.0f;
    float omega = 2.0f * (float)M_PI * freq / AUDIO_SAMPLE_RATE_EXACT;
    coeff = 2.0f * cosf(omega);
    s_prev = 0.0f;
    s_prev2 = 0.0f;
    sample_idx = 0;
    new_data = false;
}

void AudioAnalyzeToneDetect::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float x = (float)block->data[i] / 32768.0f;
        float s = x + coeff * s_prev - s_prev2;
        s_prev2 = s_prev;
        s_prev = s;
        sample_idx++;
        if (sample_idx >= 512) {
            float power = s_prev2 * s_prev2 + s_prev * s_prev - coeff * s_prev * s_prev2;
            magnitude_val = sqrtf(power > 0.0f ? power : 0.0f) / 256.0f;
            s_prev = 0.0f;
            s_prev2 = 0.0f;
            sample_idx = 0;
            new_data = true;
        }
    }
    release(block);
}
