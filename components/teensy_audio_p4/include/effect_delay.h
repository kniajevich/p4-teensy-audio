#pragma once

#include "AudioStream.h"

#define DELAY_QUEUE_NUM_BLOCKS 64

class AudioEffectDelay : public AudioStream {
public:
    AudioEffectDelay() : AudioStream(1, inputQueueArray) {}
    
    void delay(uint8_t channel, float milliseconds);
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    audio_block_t *queue[DELAY_QUEUE_NUM_BLOCKS] = {nullptr};
    uint16_t head = 0;
    uint16_t delay_blocks[8] = {0};
    uint16_t delay_samples[8] = {0};
    uint8_t channel_mask = 0;
};
