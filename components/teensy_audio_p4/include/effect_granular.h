#pragma once

#include "AudioStream.h"
#include <string.h>

class AudioEffectGranular : public AudioStream {
public:
    AudioEffectGranular() : AudioStream(1, inputQueueArray), sample_buf(nullptr), max_samples(0), write_head(0), grain_samples(1024), pitch_ratio(1.0f), mode(0), read_head1(0.0f), read_head2(0.0f) {}
    bool begin(int16_t *sample_bank, int max_len_samples);
    void stop(void);
    void beginPitchShift(float grain_length_ms);
    void beginFreeze(float grain_length_ms);
    void setPitch(float ratio);
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    int16_t *sample_buf;
    int max_samples;
    int write_head;
    int grain_samples;
    float pitch_ratio;
    int mode;
    float read_head1;
    float read_head2;
};
