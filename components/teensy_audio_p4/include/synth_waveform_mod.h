#pragma once

#include "AudioStream.h"
#include "synth_waveform.h"

class AudioSynthWaveformModulated : public AudioStream {
public:
    AudioSynthWaveformModulated() : AudioStream(2, inputQueueArray), phase_accumulator(0), phase_increment(0), magnitude(0), tone_type(WAVEFORM_SINE), modulation_factor(32768) {}

    void begin(int16_t t) { tone_type = (waveform_type_t)t; }
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
    void frequencyModulation(float octaves) {
        if (octaves < 0.0f) octaves = 0.0f;
        modulation_factor = (int32_t)(octaves * 32768.0f);
    }
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[2];
    uint32_t phase_accumulator;
    uint32_t phase_increment;
    int16_t magnitude;
    waveform_type_t tone_type;
    int32_t modulation_factor;
};
