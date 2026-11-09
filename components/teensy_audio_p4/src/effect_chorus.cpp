#include "effect_chorus.h"
#include <cmath>

bool AudioEffectChorus::begin(int16_t *delayline, int length, int voices) {
    if (!delayline || length < 256) return false;
    delay_buf = delayline;
    buf_len = length;
    voices = (voices < 1) ? 1 : ((voices > 4) ? 4 : voices);
    num_voices = voices;
    write_idx = 0;
    memset(delay_buf, 0, buf_len * sizeof(int16_t));
    return true;
}

void AudioEffectChorus::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;
    if (!delay_buf || buf_len <= 0) {
        release(block);
        return;
    }

    audio_block_t *out = allocate();
    if (!out) {
        release(block);
        return;
    }

    int base_delay = buf_len / 2;
    int mod_depth = buf_len / 4;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        delay_buf[write_idx] = block->data[i];
        int32_t sum = block->data[i];

        for (int v = 0; v < num_voices; v++) {
            lfo_phase[v] += (uint32_t)(25000000u * (v + 1));
            float mod = sinf((float)lfo_phase[v] * (2.0f * (float)M_PI / 4294967296.0f));
            int offset = base_delay + (int)(mod * mod_depth);
            int read_idx = write_idx - offset;
            while (read_idx < 0) read_idx += buf_len;
            sum += delay_buf[read_idx];
        }

        sum /= (num_voices + 1);
        if (sum > 32767) sum = 32767;
        if (sum < -32768) sum = -32768;
        out->data[i] = (int16_t)sum;
        write_idx = (write_idx + 1) % buf_len;
    }

    release(block);
    transmit(out, 0);
    release(out);
}
