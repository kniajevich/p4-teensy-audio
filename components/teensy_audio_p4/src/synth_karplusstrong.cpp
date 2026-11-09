#include "synth_karplusstrong.h"

void AudioSynthKarplusStrong::noteOn(float freq, float velocity) {
    if (freq < 45.0f) freq = 45.0f;
    if (freq > 2000.0f) freq = 2000.0f;
    if (velocity < 0.0f) velocity = 0.0f;
    if (velocity > 1.0f) velocity = 1.0f;

    buffer_len = (int)(AUDIO_SAMPLE_RATE_EXACT / freq);
    if (buffer_len > 1024) buffer_len = 1024;
    if (buffer_len < 4) buffer_len = 4;

    magnitude = (int16_t)(velocity * 32767.0f);
    index = 0;
    for (int i = 0; i < buffer_len; i++) {
        seed = seed * 1664525u + 1013904223u;
        int16_t rand_sample = (int16_t)(seed >> 16);
        buffer[i] = (int16_t)(((int32_t)rand_sample * magnitude) >> 15);
    }
}

void AudioSynthKarplusStrong::noteOff(float velocity) {
    (void)velocity;
}

void AudioSynthKarplusStrong::update(void) {
    if (buffer_len == 0) return;
    audio_block_t *block = allocate();
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int prev_idx = index - 1;
        if (prev_idx < 0) prev_idx = buffer_len - 1;
        int32_t avg = ((int32_t)buffer[index] + (int32_t)buffer[prev_idx]) >> 1;
        avg = (avg * 32600) >> 15;
        buffer[index] = (int16_t)avg;
        block->data[i] = (int16_t)avg;
        index++;
        if (index >= buffer_len) index = 0;
    }

    transmit(block, 0);
    release(block);
}
