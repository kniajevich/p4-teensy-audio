#include "effect_fade.h"

void AudioEffectFade::fadeIn(uint32_t milliseconds) {
    if (milliseconds == 0) {
        cur_gain = 65536;
        fading = false;
        return;
    }
    target_gain = 65536;
    uint32_t total_samples = (uint32_t)((float)milliseconds * (AUDIO_SAMPLE_RATE_EXACT / 1000.0f));
    if (total_samples == 0) total_samples = 1;
    step = (target_gain - cur_gain) / (int32_t)total_samples;
    if (step == 0) step = 1;
    fading = true;
}

void AudioEffectFade::fadeOut(uint32_t milliseconds) {
    if (milliseconds == 0) {
        cur_gain = 0;
        fading = false;
        return;
    }
    target_gain = 0;
    uint32_t total_samples = (uint32_t)((float)milliseconds * (AUDIO_SAMPLE_RATE_EXACT / 1000.0f));
    if (total_samples == 0) total_samples = 1;
    step = (target_gain - cur_gain) / (int32_t)total_samples;
    if (step == 0) step = -1;
    fading = true;
}

void AudioEffectFade::update(void) {
    audio_block_t *block = receiveWritable(0);
    if (!block) return;

    if (cur_gain == 0 && !fading) {
        release(block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        if (fading) {
            cur_gain += step;
            if ((step > 0 && cur_gain >= target_gain) || (step < 0 && cur_gain <= target_gain)) {
                cur_gain = target_gain;
                fading = false;
            }
        }
        block->data[i] = (int16_t)(((int32_t)block->data[i] * cur_gain) >> 16);
    }

    transmit(block, 0);
    release(block);
}
