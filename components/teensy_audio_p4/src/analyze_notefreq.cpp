#include "analyze_notefreq.h"

void AudioAnalyzeNoteFreq::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        buffer[sample_count++] = (float)block->data[i] / 32768.0f;
    }
    release(block);

    if (sample_count < 1024) return;
    sample_count = 0;

    // YIN Pitch Tracking Algorithm
    const int W = 512;
    float d[W];
    d[0] = 0.0f;

    for (int tau = 1; tau < W; tau++) {
        float sum = 0.0f;
        for (int j = 0; j < W; j++) {
            float diff = buffer[j] - buffer[j + tau];
            sum += diff * diff;
        }
        d[tau] = sum;
    }

    // Cumulative mean normalized difference
    float d_prime[W];
    d_prime[0] = 1.0f;
    float running_sum = 0.0f;

    for (int tau = 1; tau < W; tau++) {
        running_sum += d[tau];
        d_prime[tau] = (running_sum > 0.0f) ? (d[tau] * (float)tau / running_sum) : 1.0f;
    }

    // Peak picking below threshold
    int tau_found = -1;
    for (int tau = 2; tau < W - 1; tau++) {
        if (d_prime[tau] < threshold) {
            while (tau + 1 < W && d_prime[tau + 1] < d_prime[tau]) {
                tau++;
            }
            tau_found = tau;
            break;
        }
    }

    if (tau_found > 0) {
        // Parabolic interpolation for sub-sample accuracy
        float y1 = d_prime[tau_found - 1];
        float y2 = d_prime[tau_found];
        float y3 = d_prime[tau_found + 1];
        float delta = (y3 - y1) / (2.0f * (2.0f * y2 - y1 - y3));
        float refined_tau = (float)tau_found + delta;
        
        current_freq = AUDIO_SAMPLE_RATE_EXACT / refined_tau;
        current_prob = 1.0f - d_prime[tau_found];
        if (current_prob < 0.0f) current_prob = 0.0f;
        new_data = true;
    } else {
        current_prob = 0.0f;
    }
}
