#include "analyze_rms.h"

void AudioAnalyzeRMS::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t sample = block->data[i];
        sum += (uint64_t)(sample * sample);
    }
    count += AUDIO_BLOCK_SAMPLES;

    // Update RMS approx every ~50ms (2200 samples)
    if (count >= 2200) {
        float mean = (float)sum / (float)count;
        rms_val = sqrtf(mean) / 32767.0f;
        sum = 0;
        count = 0;
        new_data = true;
    }

    release(block);
}
