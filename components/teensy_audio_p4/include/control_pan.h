#pragma once

#include "AudioStream.h"

class AudioEffectPan : public AudioStream {
public:
    AudioEffectPan() : AudioStream(1, inputQueueArray), left_gain(23170), right_gain(23170) {}
    
    // Pan: -1.0 (hard left) to +1.0 (hard right), 0.0 center
    void pan(float p);
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    int32_t left_gain;
    int32_t right_gain;
};
