#pragma once

#include "AudioStream.h"

class AudioFilterLadder : public AudioStream {
public:
    AudioFilterLadder() : AudioStream(1, m_input_queue), m_cutoff(1200.0f), m_resonance(0.5f), m_drive(1.0f) {
        for (int i = 0; i < 4; i++) m_stage[i] = 0.0f;
    }
    void frequency(float freq) { m_cutoff = freq; }
    void resonance(float res) { m_resonance = res; }
    void drive(float d) { m_drive = d; }
    virtual void update();

private:
    audio_block_t* m_input_queue[1];
    float m_cutoff;
    float m_resonance;
    float m_drive;
    float m_stage[4];
};
