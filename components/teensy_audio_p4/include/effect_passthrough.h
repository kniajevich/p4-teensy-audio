#pragma once

#include "AudioStream.h"

class AudioEffectPassthrough : public AudioStream {
public:
    AudioEffectPassthrough() : AudioStream(1, inputQueueArray) {}
    virtual void update(void) override {
        audio_block_t *block = receiveReadOnly(0);
        if (!block) return;
        transmit(block, 0);
        release(block);
    }
private:
    audio_block_t *inputQueueArray[1];
};
