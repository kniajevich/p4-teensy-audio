#include "filter_biquad.h"

void AudioFilterBiquad::setLowpass(uint32_t stage, float frequency, float q) {
    if (stage >= 4) return;
    if (frequency <= 0.0f) frequency = 1.0f;
    if (frequency > AUDIO_SAMPLE_RATE_EXACT / 2.0f) frequency = AUDIO_SAMPLE_RATE_EXACT / 2.0f;
    if (q <= 0.0f) q = 0.7071f;

    float w0 = 2.0f * (float)M_PI * frequency / AUDIO_SAMPLE_RATE_EXACT;
    float alpha = sinf(w0) / (2.0f * q);
    float cos_w0 = cosf(w0);

    float a0 = 1.0f + alpha;
    float b0 = ((1.0f - cos_w0) / 2.0f) / a0;
    float b1 = (1.0f - cos_w0) / a0;
    float b2 = ((1.0f - cos_w0) / 2.0f) / a0;
    float a1 = (-2.0f * cos_w0) / a0;
    float a2 = (1.0f - alpha) / a0;

    int32_t c[5] = {
        (int32_t)(b0 * 1073741824.0f),
        (int32_t)(b1 * 1073741824.0f),
        (int32_t)(b2 * 1073741824.0f),
        (int32_t)(-a1 * 1073741824.0f),
        (int32_t)(-a2 * 1073741824.0f)
    };
    setCoefficients(stage, c);
}

void AudioFilterBiquad::setHighpass(uint32_t stage, float frequency, float q) {
    if (stage >= 4) return;
    if (frequency <= 0.0f) frequency = 1.0f;
    if (frequency > AUDIO_SAMPLE_RATE_EXACT / 2.0f) frequency = AUDIO_SAMPLE_RATE_EXACT / 2.0f;
    if (q <= 0.0f) q = 0.7071f;

    float w0 = 2.0f * (float)M_PI * frequency / AUDIO_SAMPLE_RATE_EXACT;
    float alpha = sinf(w0) / (2.0f * q);
    float cos_w0 = cosf(w0);

    float a0 = 1.0f + alpha;
    float b0 = ((1.0f + cos_w0) / 2.0f) / a0;
    float b1 = (-(1.0f + cos_w0)) / a0;
    float b2 = ((1.0f + cos_w0) / 2.0f) / a0;
    float a1 = (-2.0f * cos_w0) / a0;
    float a2 = (1.0f - alpha) / a0;

    int32_t c[5] = {
        (int32_t)(b0 * 1073741824.0f),
        (int32_t)(b1 * 1073741824.0f),
        (int32_t)(b2 * 1073741824.0f),
        (int32_t)(-a1 * 1073741824.0f),
        (int32_t)(-a2 * 1073741824.0f)
    };
    setCoefficients(stage, c);
}

void AudioFilterBiquad::setBandpass(uint32_t stage, float frequency, float q) {
    if (stage >= 4) return;
    if (frequency <= 0.0f) frequency = 1.0f;
    if (frequency > AUDIO_SAMPLE_RATE_EXACT / 2.0f) frequency = AUDIO_SAMPLE_RATE_EXACT / 2.0f;
    if (q <= 0.0f) q = 1.0f;

    float w0 = 2.0f * (float)M_PI * frequency / AUDIO_SAMPLE_RATE_EXACT;
    float alpha = sinf(w0) / (2.0f * q);
    float cos_w0 = cosf(w0);

    float a0 = 1.0f + alpha;
    float b0 = (alpha) / a0;
    float b1 = 0.0f;
    float b2 = (-alpha) / a0;
    float a1 = (-2.0f * cos_w0) / a0;
    float a2 = (1.0f - alpha) / a0;

    int32_t c[5] = {
        (int32_t)(b0 * 1073741824.0f),
        (int32_t)(b1 * 1073741824.0f),
        (int32_t)(b2 * 1073741824.0f),
        (int32_t)(-a1 * 1073741824.0f),
        (int32_t)(-a2 * 1073741824.0f)
    };
    setCoefficients(stage, c);
}

void AudioFilterBiquad::setCoefficients(uint32_t stage, const int32_t *coefficients) {
    if (stage >= 4) return;
    for (int i = 0; i < 5; i++) {
        coef[stage * 5 + i] = coefficients[i];
    }
}

void AudioFilterBiquad::update(void) {
    audio_block_t *block = receiveWritable(0);
    if (!block) return;

    for (int s = 0; s < 4; s++) {
        int32_t b0 = coef[s * 5 + 0];
        int32_t b1 = coef[s * 5 + 1];
        int32_t b2 = coef[s * 5 + 2];
        int32_t a1 = coef[s * 5 + 3];
        int32_t a2 = coef[s * 5 + 4];

        // Skip passthrough identity stage
        if (b0 == 1073741824 && b1 == 0 && b2 == 0 && a1 == 0 && a2 == 0) continue;

        int32_t x1 = state[s * 4 + 0];
        int32_t x2 = state[s * 4 + 1];
        int32_t y1 = state[s * 4 + 2];
        int32_t y2 = state[s * 4 + 3];

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            int32_t in = block->data[i];
            int64_t acc = (int64_t)in * b0 + (int64_t)x1 * b1 + (int64_t)x2 * b2 + (int64_t)y1 * a1 + (int64_t)y2 * a2;
            int32_t out = (int32_t)(acc >> 30);

            if (out > 32767) out = 32767;
            else if (out < -32768) out = -32768;

            x2 = x1;
            x1 = in;
            y2 = y1;
            y1 = out;
            block->data[i] = (int16_t)out;
        }

        state[s * 4 + 0] = x1;
        state[s * 4 + 1] = x2;
        state[s * 4 + 2] = y1;
        state[s * 4 + 3] = y2;
    }

    transmit(block, 0);
    release(block);
}
