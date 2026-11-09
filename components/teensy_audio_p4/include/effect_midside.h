#pragma once

#include "AudioStream.h"

class AudioEffectMidSide : public AudioStream {
public:
    AudioEffectMidSide() : AudioStream(2, inputQueueArray), decode_mode(false) {}
    void encode(void) { decode_mode = false; }
    void decode(void) { decode_mode = true; }
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[2];
    bool decode_mode;
};
