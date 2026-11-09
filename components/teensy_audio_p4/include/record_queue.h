#pragma once

#include "AudioStream.h"

class AudioRecordQueue : public AudioStream {
public:
    AudioRecordQueue() : AudioStream(1, inputQueueArray), is_enabled(false), q_head(0), q_tail(0) {
        for (int i = 0; i < 8; i++) queue[i] = nullptr;
    }
    virtual ~AudioRecordQueue();
    void begin(void) { is_enabled = true; }
    void clear(void);
    void end(void) { is_enabled = false; clear(); }
    int available(void);
    int16_t * readBuffer(void);
    void freeBuffer(void);
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    bool is_enabled;
    audio_block_t *queue[8];
    uint8_t q_head;
    uint8_t q_tail;
};
