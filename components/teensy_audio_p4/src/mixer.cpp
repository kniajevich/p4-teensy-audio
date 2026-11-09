#include "mixer.h"
#include <algorithm>
#include <cmath>

/* Helper macro to convert floating‑point gain to Q15 integer. */
#define GAIN_FLOAT_TO_Q15(x) static_cast<int16_t>(std::clamp((x), 0.0f, 32767.0f))

AudioMixer4::AudioMixer4()
{
    // Initialise all gains to maximum (no attenuation)
    for (int i = 0; i < 4; ++i) {
        gains_[i] = 32767;
    }
}

void AudioMixer4::gain(uint8_t channel, float level)
{
    if (channel > 3) return; // ignore invalid channel
    gains_[channel] = GAIN_FLOAT_TO_Q15(level);
}

/**
 * Mix four samples.
 *
 * The algorithm multiplies each sample by its channel gain using the
 * __SMLALD intrinsic (multiply‑accumulate without overflow) and then
 * right‑shifts the product by 15 to convert back from Q15.
 * The summed result is saturated to the int16_t range with __SSAT.
 */
int16_t AudioMixer4::mix(int16_t in0, int16_t in1, int16_t in2, int16_t in3)
{
    int32_t acc = 0;

    // Channel 0
    acc = __SMLALD(in0, gains_[0], acc);
    // Channel 1
    acc = __SMLALD(in1, gains_[1], acc);
    // Channel 2
    acc = __SMLALD(in2, gains_[2], acc);
    // Channel 3
    acc = __SMLALD(in3, gains_[3], acc);

    // Convert from Q30 (product of two Q15 values) to Q15 by shifting 15 bits.
    acc >>= 15;

    // Saturate to int16_t range.
    return static_cast<int16_t>(__SSAT(acc, 16));
}