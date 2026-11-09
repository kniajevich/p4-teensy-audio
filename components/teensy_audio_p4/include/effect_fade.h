#pragma once

#include "AudioStream.h"

class AudioEffectFade : public AudioStream {
public:
    AudioEffectFade() : AudioStream(1, inputQueueArray), cur_gain(65536), step(0), target_gain(65536), fading(false) {}
    void fadeIn(uint32_t milliseconds);
    void fadeOut(uint32_t milliseconds);
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    int32_t cur_gain;
    int32_t step;
    int32_t target_gain;
    bool fading;
};
