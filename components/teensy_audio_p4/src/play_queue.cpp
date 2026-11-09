#include "play_queue.h"

AudioPlayQueue::~AudioPlayQueue() {
    if (user_block) release(user_block);
    while (q_head != q_tail) {
        if (queue[q_tail]) release(queue[q_tail]);
        q_tail = (q_tail + 1) % 4;
    }
}

int16_t * AudioPlayQueue::getBuffer(void) {
    if (user_block) return user_block->data;
    user_block = allocate();
    return user_block ? user_block->data : nullptr;
}

void AudioPlayQueue::playBuffer(void) {
    if (!user_block) return;
    uint8_t next_head = (q_head + 1) % 4;
    if (next_head == q_tail) {
        release(user_block);
        user_block = nullptr;
        return;
    }
    queue[q_head] = user_block;
    q_head = next_head;
    user_block = nullptr;
}

bool AudioPlayQueue::available(void) {
    return ((q_head + 1) % 4) != q_tail;
}

void AudioPlayQueue::update(void) {
    if (q_head == q_tail) return;
    audio_block_t *block = queue[q_tail];
    q_tail = (q_tail + 1) % 4;
    if (block) {
        transmit(block, 0);
        release(block);
    }
}
