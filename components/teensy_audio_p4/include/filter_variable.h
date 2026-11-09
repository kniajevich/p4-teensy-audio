#ifndef FILTER_VARIABLE_H
#define FILTER_VARIABLE_H

#include <cstdint>
#include "arm_math_mock.h"

/**
 * @brief Chamberlin State Variable Filter (SVF) implementation.
 *
 * This filter provides three simultaneous outputs:
 *   0 – Low‑pass
 *   1 – Band‑pass
 *   2 – High‑pass
 *
 * The filter works on 16‑bit signed audio samples.
 */
class AudioFilterStateVariable {
public:
    /** Construct a filter with a given sample rate. */
    explicit AudioFilterStateVariable(float sampleRate = 44100.0f);

    /** Set the cutoff frequency (Hz). */
    void setFrequency(float frequency);

    /** Set the resonance (Q factor). */
    void setResonance(float resonance);

    /** Process a single input sample and obtain the selected output.
     *
     * @param input   Input audio sample (int16_t).
     * @param mode    0 = low‑pass, 1 = band‑pass, 2 = high‑pass.
     * @return        Filtered sample (int16_t) saturated to the int16 range.
     */
    int16_t update(int16_t input, uint8_t mode = 0);

private:
    // Internal state variables (Q15 fixed‑point)
    int32_t low;   // low‑pass accumulator
    int32_t band;  // band‑pass accumulator

    // Coefficients in Q15 format
    int16_t f;     // frequency coefficient (0..32767)
    int16_t q;     // resonance coefficient (0..32767)

    const float sampleRate_;
    void recalcCoefficients();
};

#endif // FILTER_VARIABLE_H