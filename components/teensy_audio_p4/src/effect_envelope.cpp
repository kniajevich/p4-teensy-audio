#include "effect_envelope.h"
#include "arm_math_mock.h"

// Helper to clamp value to 16-bit signed range
static inline int16_t clamp16(int32_t val) {
    if (val > 32767) return 32767;
    if (val < -32768) return -32768;
    return (int16_t)val;
}

AudioEffectEnvelope::AudioEffectEnvelope()
    : AudioStream(1, inputQueueArray),
      state(STATE_IDLE),
      count(0),
      mult(0),
      inc(0),
      delay_count(0),
      attack_count((int32_t)(AUDIO_SAMPLE_RATE_EXACT * 0.0105f)), // 10.5ms default
      hold_count(0),
      decay_count((int32_t)(AUDIO_SAMPLE_RATE_EXACT * 0.035f)),   // 35ms default
      sustain_mult(16384),                                         // 0.5 default
      release_count((int32_t)(AUDIO_SAMPLE_RATE_EXACT * 0.300f))   // 300ms default
{
    inputQueueArray[0] = nullptr;
}

void AudioEffectEnvelope::noteOn() {
    if (state == STATE_IDLE) {
        if (delay_count > 0) {
            state = STATE_DELAY;
            count = delay_count;
        } else {
            state = STATE_ATTACK;
            count = attack_count;
            if (attack_count > 0) {
                // Calculate increment per sample to reach 32767 (1.0)
                // Using Q15 fixed point for precision
                inc = ((32767 - mult) << 15) / attack_count;
            } else {
                mult = 32767;
                state = STATE_HOLD;
                count = hold_count;
            }
        }
    }
}

void AudioEffectEnvelope::noteOff() {
    if (state != STATE_IDLE) {
        state = STATE_RELEASE;
        count = release_count;
        if (release_count > 0) {
            // Calculate decrement per sample to reach 0
            inc = -((mult << 15) / release_count);
        } else {
            mult = 0;
            state = STATE_IDLE;
        }
    }
}

void AudioEffectEnvelope::delay(float ms) {
    if (ms < 0.0f) ms = 0.0f;
    delay_count = (int32_t)(AUDIO_SAMPLE_RATE_EXACT * (ms * 0.001f));
}

void AudioEffectEnvelope::attack(float ms) {
    if (ms < 0.0f) ms = 0.0f;
    attack_count = (int32_t)(AUDIO_SAMPLE_RATE_EXACT * (ms * 0.001f));
}

void AudioEffectEnvelope::hold(float ms) {
    if (ms < 0.0f) ms = 0.0f;
    hold_count = (int32_t)(AUDIO_SAMPLE_RATE_EXACT * (ms * 0.001f));
}

void AudioEffectEnvelope::decay(float ms) {
    if (ms < 0.0f) ms = 0.0f;
    decay_count = (int32_t)(AUDIO_SAMPLE_RATE_EXACT * (ms * 0.001f));
}

void AudioEffectEnvelope::sustain(float level) {
    if (level < 0.0f) level = 0.0f;
    if (level > 1.0f) level = 1.0f;
    sustain_mult = (int32_t)(level * 32767.0f);
}

void AudioEffectEnvelope::release(float ms) {
    if (ms < 0.0f) ms = 0.0f;
    release_count = (int32_t)(AUDIO_SAMPLE_RATE_EXACT * (ms * 0.001f));
}

void AudioEffectEnvelope::update() {
    audio_block_t *block = receiveWritable(0);
    if (!block) return;

    if (state == STATE_IDLE) {
        // Zero out the block if envelope is idle
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            block->data[i] = 0;
        }
        transmit(block, 0);
        release(block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        switch (state) {
            case STATE_DELAY:
                if (--count <= 0) {
                    state = STATE_ATTACK;
                    count = attack_count;
                    if (attack_count > 0) {
                        inc = ((32767 - mult) << 15) / attack_count;
                    } else {
                        mult = 32767;
                        state = STATE_HOLD;
                        count = hold_count;
                    }
                }
                break;

            case STATE_ATTACK:
                mult += (inc >> 15);
                if (--count <= 0 || mult >= 32767) {
                    mult = 32767;
                    state = STATE_HOLD;
                    count = hold_count;
                }
                break;

            case STATE_HOLD:
                if (--count <= 0) {
                    state = STATE_DECAY;
                    count = decay_count;
                    if (decay_count > 0) {
                        inc = -(((32767 - sustain_mult) << 15) / decay_count);
                    } else {
                        mult = sustain_mult;
                        state = STATE_SUSTAIN;
                    }
                }
                break;

            case STATE_DECAY:
                mult += (inc >> 15);
                if (--count <= 0 || mult <= sustain_mult) {
                    mult = sustain_mult;
                    state = STATE_SUSTAIN;
                }
                break;

            case STATE_SUSTAIN:
                // Hold at sustain level until noteOff
                break;

            case STATE_RELEASE:
                mult += (inc >> 15);
                if (--count <= 0 || mult <= 0) {
                    mult = 0;
                    state = STATE_IDLE;
                }
                break;

            default:
                break;
        }

        // Apply envelope to sample
        // mult is Q15 (0 to 32767)
        // block->data[i] is Q15 (-32768 to 32767)
        // Result: (sample * mult) >> 15
        int32_t val = (block->data[i] * mult) >> 15;
        block->data[i] = clamp16(val);
    }

    transmit(block, 0);
    release(block);
}