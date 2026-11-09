#include "effect_envelope.h"
#include "arm_math_mock.h"

AudioEffectEnvelope::AudioEffectEnvelope()
    : AudioStream(1, inputQueueArray),
      state(STATE_IDLE),
      count(0),
      mult(0),
      inc(0),
      delay_count(0),
      attack_count(AUDIO_SAMPLE_RATE_EXACT * 0.0105f),
      hold_count(0),
      decay_count(AUDIO_SAMPLE_RATE_EXACT * 0.035f),
      sustain_mult(16384),
      release_count(AUDIO_SAMPLE_RATE_EXACT * 0.300f) {
    inputQueueArray[0] = nullptr;
}

void AudioEffectEnvelope::noteOn() {
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

void AudioEffectEnvelope::noteOff() {
    state = STATE_RELEASE;
    count = release_count;
    if (release_count > 0) {
        inc = -((mult << 15) / release_count);
    } else {
        mult = 0;
        state = STATE_IDLE;
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
        AudioStream::release(block);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        switch (state) {
            case STATE_IDLE:
                block->data[i] = 0;
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

        int32_t val = (block->data[i] * mult) >> 15;
        block->data[i] = (int16_t)__SSAT(val, 16);
    }

    transmit(block, 0);
    AudioStream::release(block);
}