#include "effect_freeverb.h"

AudioEffectFreeverb::AudioEffectFreeverb() : AudioStream(1, inputQueueArray), roomsize_val(16384), damp_val(6553), use_golden_tuning(false) {
    const int comb_tuning[8] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
    const int allpass_tuning[4] = {556, 441, 341, 225};
    memset(comb_buf, 0, sizeof(comb_buf));
    memset(comb_filter, 0, sizeof(comb_filter));
    memset(comb_idx, 0, sizeof(comb_idx));
    for (int i = 0; i < 8; i++) comb_len[i] = comb_tuning[i];
    memset(allpass_buf, 0, sizeof(allpass_buf));
    memset(allpass_idx, 0, sizeof(allpass_idx));
    for (int i = 0; i < 4; i++) allpass_len[i] = allpass_tuning[i];
}

void AudioEffectFreeverb::roomsize(float r) {
    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    roomsize_val = (int16_t)(r * 32000.0f);
}

void AudioEffectFreeverb::damping(float d) {
    if (d < 0.0f) d = 0.0f;
    if (d > 1.0f) d = 1.0f;
    damp_val = (int16_t)(d * 16384.0f);
}

void AudioEffectFreeverb::goldenTuning(bool enable) {
    use_golden_tuning = enable;
    const int comb_standard[8] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
    const int allpass_standard[4] = {556, 441, 341, 225};
    // Golden ratio (phi) distributed mutually prime delay lines to eliminate comb resonance
    const int comb_phi[8] = {1087, 1153, 1229, 1319, 1399, 1489, 1567, 1663};
    const int allpass_phi[4] = {577, 439, 347, 223};

    for (int i = 0; i < 8; i++) {
        comb_len[i] = enable ? comb_phi[i] : comb_standard[i];
        comb_idx[i] = 0;
        comb_filter[i] = 0;
    }
    for (int i = 0; i < 4; i++) {
        allpass_len[i] = enable ? allpass_phi[i] : allpass_standard[i];
        allpass_idx[i] = 0;
    }
}

void AudioEffectFreeverb::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;

    audio_block_t *out_l = allocate();
    audio_block_t *out_r = allocate();
    if (!out_l || !out_r) {
        if (out_l) release(out_l);
        if (out_r) release(out_r);
        release(block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int32_t input = block->data[i] >> 2;
        int32_t out_comb_l = 0;
        int32_t out_comb_r = 0;

        for (int c = 0; c < 8; c++) {
            int idx = comb_idx[c];
            int32_t output = comb_buf[c][idx];
            comb_filter[c] = (int16_t)(((output * (32767 - damp_val)) + ((int32_t)comb_filter[c] * damp_val)) >> 15);
            comb_buf[c][idx] = (int16_t)(input + (((int32_t)comb_filter[c] * roomsize_val) >> 15));
            comb_idx[c] = (idx + 1 >= comb_len[c]) ? 0 : idx + 1;
            if (c & 1) out_comb_r += output; else out_comb_l += output;
        }

        int32_t ap_l = out_comb_l >> 2;
        int32_t ap_r = out_comb_r >> 2;
        for (int a = 0; a < 4; a++) {
            int idx = allpass_idx[a];
            int32_t buf_out = allpass_buf[a][idx];
            allpass_buf[a][idx] = (int16_t)(ap_l + (buf_out >> 1));
            ap_l = -ap_l + buf_out;
            allpass_idx[a] = (idx + 1 >= allpass_len[a]) ? 0 : idx + 1;
        }

        out_l->data[i] = (int16_t)(ap_l > 32767 ? 32767 : (ap_l < -32768 ? -32768 : ap_l));
        out_r->data[i] = (int16_t)(ap_r > 32767 ? 32767 : (ap_r < -32768 ? -32768 : ap_r));
    }

    release(block);
    transmit(out_l, 0);
    transmit(out_r, 1);
    release(out_l);
    release(out_r);
}
