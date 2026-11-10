#include "AudioFilterLadder.h"
#include <cmath>

static inline float fast_tanh(float x) {
    float x2 = x * x;
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

void AudioFilterLadder::update() {
    audio_block_t* block = receiveWritable(0);
    if (!block) return;

    float fs = 44100.0f;
    float fc = m_cutoff / fs;
    if (fc > 0.45f) fc = 0.45f;
    float f = fc * 1.16f;
    float fb = m_resonance * (1.0f - 0.15f * f * f);

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float input = (block->data[i] / 32768.0f) * m_drive;
        input -= m_stage[3] * fb;
        input = fast_tanh(input);

        m_stage[0] += f * (input - m_stage[0]);
        m_stage[1] += f * (m_stage[0] - m_stage[1]);
        m_stage[2] += f * (m_stage[1] - m_stage[2]);
        m_stage[3] += f * (m_stage[2] - m_stage[3]);

        float out = m_stage[3];
        if (out > 1.0f) out = 1.0f;
        if (out < -1.0f) out = -1.0f;
        block->data[i] = static_cast<int16_t>(out * 32767.0f);
    }
    transmit(block, 0);
    release(block);
}
