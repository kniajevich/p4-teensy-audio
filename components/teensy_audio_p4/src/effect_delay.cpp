#include "effect_delay.h"

void AudioEffectDelay::delay(uint8_t channel, float milliseconds) {
    if (channel >= 8) return;
    if (milliseconds <= 0.0f) {
        channel_mask &= ~(1 << channel);
        return;
    }
    uint32_t total_samples = (uint32_t)(milliseconds * (AUDIO_SAMPLE_RATE_EXACT / 1000.0f));
    uint16_t blocks = total_samples / AUDIO_BLOCK_SAMPLES;
    if (blocks >= DELAY_QUEUE_NUM_BLOCKS) blocks = DELAY_QUEUE_NUM_BLOCKS - 1;
    
    delay_blocks[channel] = blocks;
    delay_samples[channel] = total_samples % AUDIO_BLOCK_SAMPLES;
    channel_mask |= (1 << channel);
}

void AudioEffectDelay::update(void) {
    audio_block_t *in = receiveReadOnly(0);
    
    // Store block into circular queue
    if (queue[head]) release(queue[head]);
    queue[head] = in;

    for (uint8_t ch = 0; ch < 8; ch++) {
        if (!(channel_mask & (1 << ch))) continue;

        audio_block_t *out = allocate();
        if (!out) continue;

        uint16_t target_blk_idx = (head + DELAY_QUEUE_NUM_BLOCKS - delay_blocks[ch]) % DELAY_QUEUE_NUM_BLOCKS;
        audio_block_t *src = queue[target_blk_idx];

        if (src) {
            for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
                out->data[i] = src->data[i];
            }
        } else {
            for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
                out->data[i] = 0;
            }
        }

        transmit(out, ch);
        release(out);
    }

    head = (head + 1) % DELAY_QUEUE_NUM_BLOCKS;
}
