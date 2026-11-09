#include "synth_tonesweep.h"
#include <cmath>

bool AudioSynthToneSweep::play(float level, float freq_start, float freq_end, float t_time) {
    if (level < 0.0f) level = 0.0f;
    if (level > 1.0f) level = 1.0f;
    if (t_time <= 0.0f) return false;

    tone_amp = level;
    tone_freq = freq_start;
    tone_hi = freq_end;
    tone_incr = (freq_end - freq_start) / (t_time * AUDIO_SAMPLE_RATE_EXACT);
    phase_acc = 0;
    sweep_busy = true;
    return true;
}

void AudioSynthToneSweep::update(void) {
    if (!sweep_busy) return;
    audio_block_t *block = allocate();
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        uint32_t phase_inc = (uint32_t)(tone_freq * (4294967296.0f / AUDIO_SAMPLE_RATE_EXACT));
        phase_acc += phase_inc;
        float sin_val = sinf((float)phase_acc * (2.0f * (float)M_PI / 4294967296.0f));
        block->data[i] = (int16_t)(sin_val * tone_amp * 32767.0f);
        tone_freq += tone_incr;
        if ((tone_incr > 0.0f && tone_freq >= tone_hi) || (tone_incr < 0.0f && tone_freq <= tone_hi)) {
            sweep_busy = false;
            for (int j = i + 1; j < AUDIO_BLOCK_SAMPLES; j++) block->data[j] = 0;
            break;
        }
    }

    transmit(block, 0);
    release(block);
}
