#include "effect_granular.h"
#include <cmath>

bool AudioEffectGranular::begin(int16_t *sample_bank, int max_len_samples) {
    if (!sample_bank || max_len_samples < 256) return false;
    sample_buf = sample_bank;
    max_samples = max_len_samples;
    write_head = 0;
    mode = 0;
    read_head1 = 0.0f;
    read_head2 = (float)(grain_samples / 2);
    memset(sample_buf, 0, max_samples * sizeof(int16_t));
    return true;
}

void AudioEffectGranular::stop(void) {
    mode = 0;
}

void AudioEffectGranular::beginPitchShift(float grain_length_ms) {
    if (grain_length_ms < 5.0f) grain_length_ms = 5.0f;
    grain_samples = (int)(grain_length_ms * (AUDIO_SAMPLE_RATE_EXACT / 1000.0f));
    if (grain_samples > max_samples / 2) grain_samples = max_samples / 2;
    if (grain_samples < 64) grain_samples = 64;
    read_head1 = 0.0f;
    read_head2 = (float)(grain_samples / 2);
    mode = 1;
}

void AudioEffectGranular::beginFreeze(float grain_length_ms) {
    beginPitchShift(grain_length_ms);
    mode = 2;
}

void AudioEffectGranular::setPitch(float ratio) {
    if (ratio < 0.125f) ratio = 0.125f;
    if (ratio > 8.0f) ratio = 8.0f;
    pitch_ratio = ratio;
}

void AudioEffectGranular::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;
    if (!sample_buf || mode == 0) {
        transmit(block, 0);
        release(block);
        return;
    }

    audio_block_t *out = allocate();
    if (!out) {
        release(block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        if (mode != 2) {
            sample_buf[write_head] = block->data[i];
            write_head = (write_head + 1) % max_samples;
        }

        float win1 = 1.0f - fabsf((read_head1 / (float)grain_samples) * 2.0f - 1.0f);
        float win2 = 1.0f - fabsf((read_head2 / (float)grain_samples) * 2.0f - 1.0f);

        int idx1 = (write_head - grain_samples + (int)read_head1 + max_samples) % max_samples;
        int idx2 = (write_head - grain_samples + (int)read_head2 + max_samples) % max_samples;

        float s = (float)sample_buf[idx1] * win1 + (float)sample_buf[idx2] * win2;
        if (s > 32767.0f) s = 32767.0f;
        if (s < -32768.0f) s = -32768.0f;
        out->data[i] = (int16_t)s;

        read_head1 += pitch_ratio;
        if (read_head1 >= (float)grain_samples) read_head1 -= (float)grain_samples;
        read_head2 += pitch_ratio;
        if (read_head2 >= (float)grain_samples) read_head2 -= (float)grain_samples;
    }

    release(block);
    transmit(out, 0);
    release(out);
}
