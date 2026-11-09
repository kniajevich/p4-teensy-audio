#ifndef MIXER_H
#define MIXER_H

#include <cstdint>
#include "arm_math_mock.h"

/**
 * @brief 4‑channel audio mixer.
 *
 * Each channel has an independent gain (0 … 32767). The mixer sums the
 * scaled samples and saturates the result to the int16_t range.
 */
class AudioMixer4 {
public:
    AudioMixer4();

    /** Set gain for a specific channel.
     *
     * @param channel Channel index (0‑3).
     * @param level   Gain level, range 0.0 … 32767.0 (floating point for API compatibility).
     */
    void gain(uint8_t channel, float level);

    /** Mix four input samples and return the saturated result. */
    int16_t mix(int16_t in0, int16_t in1, int16_t in2, int16_t in3);

private:
    // Gains stored as Q15 (int16_t) – level is scaled to 0 … 32767.
    int16_t gains_[4];
};

#endif // MIXER_H