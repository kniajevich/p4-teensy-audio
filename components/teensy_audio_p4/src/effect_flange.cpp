#include "effect_flange.h"
#include <cmath>

bool AudioEffectFlange::begin(int16_t *delayline, int length, int offset, int depth, float rate) {
    if (!delayline || length < 128) return false;
    delay_buf = delayline;
    buf_len = length;
    offset_idx = offset;
    depth_idx = depth;
    lfo_inc = (uint32_t)(rate * (4294967296.0f / AUDIO_SAMPLE_RATE_EXACT));
    lfo_phase = 0;
    write_idx = 0;
    memset(delay_buf, 0, buf_len * sizeof(int16_t));
    return true;
}

void AudioEffectFlange::update(void) {
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

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        delay_buf[write_idx] = block->data[i];
        lfo_phase += lfo_inc;
        float mod = (sinf((float)lfo_phase * (2.0f * (float)M_PI / 4294967296.0f)) + 1.0f) * 0.5f;
        int cur_delay = offset_idx + (int)(mod * depth_idx);
        int read_idx = write_idx - cur_delay;
        while (read_idx < 0) read_idx += buf_len;

        int32_t sum = ((int32_t)block->data[i] + (int32_t)delay_buf[read_idx]) >> 1;
        out->data[i] = (int16_t)sum;
        write_idx = (write_idx + 1) % buf_len;
    }

    release(block);
    transmit(out, 0);
    release(out);
}
