#pragma once

#include "AudioStream.h"

class AudioSynthToneSweep : public AudioStream {
public:
    AudioSynthToneSweep() : AudioStream(0, nullptr), sweep_busy(false), tone_amp(0.0f), tone_freq(0.0f), tone_hi(0.0f), tone_incr(0.0f), phase_acc(0) {}
    bool play(float level, float freq_start, float freq_end, float t_time);
    bool isPlaying(void) const { return sweep_busy; }
    virtual void update(void) override;
private:
    bool sweep_busy;
    float tone_amp;
    float tone_freq;
    float tone_hi;
    float tone_incr;
    uint32_t phase_acc;
};
