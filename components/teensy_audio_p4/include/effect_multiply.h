#pragma once

#include "AudioStream.h"

class AudioEffectMultiply : public AudioStream {
public:
    AudioEffectMultiply() : AudioStream(2, inputQueueArray) {}
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[2];
};
