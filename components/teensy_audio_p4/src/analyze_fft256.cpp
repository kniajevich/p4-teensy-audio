#include "analyze_fft256.h"

static void compute_fft256(float *real, float *imag) {
    int n = 256;
    int j = 0;
    for (int i = 0; i < n - 1; i++) {
        if (i < j) {
            float tr = real[i]; real[i] = real[j]; real[j] = tr;
            float ti = imag[i]; imag[i] = imag[j]; imag[j] = ti;
        }
        int k = n / 2;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
    for (int len = 2; len <= n; len <<= 1) {
        float ang = -2.0f * (float)M_PI / len;
        float wlen_r = cosf(ang);
        float wlen_i = sinf(ang);
        for (int i = 0; i < n; i += len) {
            float w_r = 1.0f;
            float w_i = 0.0f;
            for (int k = 0; k < len / 2; k++) {
                float u_r = real[i + k];
                float u_i = imag[i + k];
                float v_r = real[i + k + len / 2] * w_r - imag[i + k + len / 2] * w_i;
                float v_i = real[i + k + len / 2] * w_i + imag[i + k + len / 2] * w_r;
                real[i + k] = u_r + v_r;
                imag[i + k] = u_i + v_i;
                real[i + k + len / 2] = u_r - v_r;
                imag[i + k + len / 2] = u_i - v_i;
                float nw_r = w_r * wlen_r - w_i * wlen_i;
                w_i = w_r * wlen_i + w_i * wlen_r;
                w_r = nw_r;
            }
        }
    }
}

void AudioAnalyzeFFT256::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        buffer[sample_count++] = (float)block->data[i] / 32768.0f;
    }
    release(block);

    if (sample_count >= 256) {
        sample_count = 0;
        float imag[256] = {0.0f};
        compute_fft256(buffer, imag);
        for (int i = 0; i < 128; i++) {
            output[i] = sqrtf(buffer[i] * buffer[i] + imag[i] * imag[i]) / 128.0f;
        }
        new_data = true;
    }
}
