#ifndef SYNTH_SINE_H
#define SYNTH_SINE_H

#include "AudioStream.h"
#include <stdint.h>

class AudioSynthWaveformSine : public AudioStream {
public:
    AudioSynthWaveformSine() : AudioStream(0, nullptr), phase_accumulator(0), phase_increment(0), magnitude(16384) {}
    void frequency(float freq);
    void amplitude(float n);
    virtual void update() override;

private:
    uint32_t phase_accumulator;
    uint32_t phase_increment;
    int16_t magnitude;
};

#endif // SYNTH_SINE_H
