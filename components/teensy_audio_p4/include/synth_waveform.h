#pragma once

#include "AudioStream.h"

enum short {
    WAVEFORM_SINE = 0,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_TRIANGLE,
    WAVEFORM_PULSE
};

class AudioSynthWaveform : public AudioStream {
public:
    AudioSynthWaveform() : AudioStream(0, nullptr), phase_accumulator(0), phase_increment(0), magnitude(0), tone_type(WAVEFORM_SINE), pulse_width(0x80000000) {}
    
    void begin(short t) {
        tone_type = t;
    }
    void frequency(float freq) {
        if (freq < 0.0f) freq = 0.0f;
        else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2) freq = AUDIO_SAMPLE_RATE_EXACT / 2;
        phase_increment = (uint32_t)(freq * (4294967296.0f / AUDIO_SAMPLE_RATE_EXACT));
    }
    void amplitude(float n) {
        if (n < 0.0f) n = 0.0f;
        else if (n > 1.0f) n = 1.0f;
        magnitude = (int16_t)(n * 32767.0f);
    }
    void pulseWidth(float n) {
        if (n < 0.0f) n = 0.0f;
        else if (n > 1.0f) n = 1.0f;
        pulse_width = (uint32_t)(n * 4294967296.0f);
    }
    virtual void update(void) override;

private:
    uint32_t phase_accumulator;
    uint32_t phase_increment;
    int16_t magnitude;
    short tone_type;
    uint32_t pulse_width;
};
