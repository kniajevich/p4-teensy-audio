#pragma once

#include "AudioStream.h"

class AudioPlayQueue : public AudioStream {
public:
    AudioPlayQueue() : AudioStream(0, nullptr), user_block(nullptr), q_head(0), q_tail(0) {
        for (int i = 0; i < 4; i++) queue[i] = nullptr;
    }
    virtual ~AudioPlayQueue();
    int16_t * getBuffer(void);
    void playBuffer(void);
    bool available(void);
    virtual void update(void) override;
private:
    audio_block_t *user_block;
    audio_block_t *queue[4];
    uint8_t q_head;
    uint8_t q_tail;
};
