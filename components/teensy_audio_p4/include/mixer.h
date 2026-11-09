#ifndef MIXER_H
#define MIXER_H

#include "AudioStream.h"
#include <stdint.h>

/**
 * @brief 4‑channel audio mixer.
 *
 * The class inherits publicly from AudioStream as required by the Teensy
 * Audio API and the ESP‑32‑P4 port.  The `gain()` method sets the per‑channel
 * multiplier used during mixing.
 */
class AudioMixer4 : public AudioStream {
public:
    AudioMixer4();
    virtual ~AudioMixer4() {}

    /**
     * @brief Set the gain for a specific channel.
     *
     * @param channel Zero‑based channel index (0‑3).
     * @param gain    Linear gain factor (0.0 … 1.0).
     */
    void gain(unsigned int channel, float gain);

    /** @brief Called by the audio engine each processing block. */
    virtual void update() override;

private:
    /** Queue of incoming audio blocks, one per channel. */
    audio_block_t *inputQueueArray[4];

    /** Fixed‑point multiplier for each channel (Q1.31 format). */
    int32_t multiplier[4];
};

#endif // MIXER_H