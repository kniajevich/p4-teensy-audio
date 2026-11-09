#include "record_queue.h"

AudioRecordQueue::~AudioRecordQueue() {
    clear();
}

void AudioRecordQueue::clear(void) {
    while (q_head != q_tail) {
        if (queue[q_tail]) release(queue[q_tail]);
        queue[q_tail] = nullptr;
        q_tail = (q_tail + 1) % 8;
    }
}

int AudioRecordQueue::available(void) {
    int count = q_head - q_tail;
    if (count < 0) count += 8;
    return count;
}

int16_t * AudioRecordQueue::readBuffer(void) {
    if (q_head == q_tail) return nullptr;
    return queue[q_tail] ? queue[q_tail]->data : nullptr;
}

void AudioRecordQueue::freeBuffer(void) {
    if (q_head == q_tail) return;
    if (queue[q_tail]) {
        release(queue[q_tail]);
        queue[q_tail] = nullptr;
    }
    q_tail = (q_tail + 1) % 8;
}

void AudioRecordQueue::update(void) {
    audio_block_t *block = receiveReadOnly(0);
    if (!block) return;
    if (!is_enabled) {
        release(block);
        return;
    }
    uint8_t next_head = (q_head + 1) % 8;
    if (next_head == q_tail) {
        release(block);
        return;
    }
    queue[q_head] = block;
    q_head = next_head;
}
