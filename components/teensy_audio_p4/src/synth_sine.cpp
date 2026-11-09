#include "synth_sine.h"
#include <math.h>

void AudioSynthWaveformSine::frequency(float freq) {
    if (freq < 0.0f) freq = 0.0f;
    if (freq > 20000.0f) freq = 20000.0f;
    phase_increment = (uint32_t)((freq * 4294967296.0f) / AUDIO_SAMPLE_RATE_EXACT);
}

void AudioSynthWaveformSine::amplitude(float n) {
    if (n < 0.0f) n = 0.0f;
    if (n > 1.0f) n = 1.0f;
    magnitude = (int16_t)(n * 32767.0f);
}

void AudioSynthWaveformSine::update() {
    audio_block_t *block = allocate();
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float angle = (float)phase_accumulator * (2.0f * M_PI / 4294967296.0f);
        block->data[i] = (int16_t)(sinf(angle) * magnitude);
        phase_accumulator += phase_increment;
    }

    transmit(block, 0);
    release(block);
}
