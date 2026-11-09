#include "analyze_peak.h"

void AudioAnalyzePeak::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;

    int16_t max_val = max_sample;
    int16_t min_val = min_sample;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int16_t sample = block->data[i];
        if (sample > max_val) max_val = sample;
        if (sample < min_val) min_val = sample;
    }

    max_sample = max_val;
    min_sample = min_val;
    new_data = true;

    release(block);
}
