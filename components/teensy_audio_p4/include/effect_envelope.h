#ifndef EFFECT_ENVELOPE_H
#define EFFECT_ENVELOPE_H

#include "AudioStream.h"
#include <stdint.h>

class AudioEffectEnvelope : public AudioStream {
public:
    enum State {
        STATE_IDLE,
        STATE_DELAY,
        STATE_ATTACK,
        STATE_HOLD,
        STATE_DECAY,
        STATE_SUSTAIN,
        STATE_RELEASE
    };

    AudioEffectEnvelope();
    virtual ~AudioEffectEnvelope() {}

    void noteOn();
    void noteOff();

    void delay(float milliseconds);
    void attack(float milliseconds);
    void hold(float milliseconds);
    void decay(float milliseconds);
    void sustain(float level);
    void release(float milliseconds);

    bool isActive() const { return state != STATE_IDLE; }
    
    // Get current envelope value for debugging/visualization (0.0 to 1.0)
    float getCurrentLevel() const {
        return (float)mult / 32767.0f;
    }

    virtual void update() override;

private:
    audio_block_t *inputQueueArray[1];
    State state;

    int32_t count;
    int32_t mult;
    int32_t inc;

    int32_t delay_count;
    int32_t attack_count;
    int32_t hold_count;
    int32_t decay_count;
    int32_t sustain_mult;
    int32_t release_count;
};

#endif // EFFECT_ENVELOPE_H