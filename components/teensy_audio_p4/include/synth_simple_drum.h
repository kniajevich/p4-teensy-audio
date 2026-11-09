#pragma once

#include "AudioStream.h"

class AudioSynthSimpleDrum : public AudioStream {
public:
    AudioSynthSimpleDrum() : AudioStream(0, nullptr), base_freq(60.0f), cur_freq(60.0f), amp(0.0f), decay_rate(0.999f), noise_mix(0.15f), pitch_depth(2.5f), phase(0), seed(12345), active(false) {}
    void frequency(float f) { base_freq = (f < 20.0f) ? 20.0f : f; }
    void length(int ms);
    void secondMix(float level) { noise_mix = (level < 0.0f) ? 0.0f : ((level > 1.0f) ? 1.0f : level); }
    void pitchMod(float depth) { pitch_depth = (depth < 0.0f) ? 0.0f : depth; }
    void noteOn(void);
    virtual void update(void) override;
private:
    float base_freq;
    float cur_freq;
    float amp;
    float decay_rate;
    float noise_mix;
    float pitch_depth;
    uint32_t phase;
    uint32_t seed;
    bool active;
};
