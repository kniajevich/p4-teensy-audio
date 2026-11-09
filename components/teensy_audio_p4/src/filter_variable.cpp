#include "filter_variable.h"
#include <cmath>
#include <algorithm>

/* Helper macros for fixed‑point conversion */
#define FLOAT_TO_Q15(x) static_cast<int16_t>(std::clamp((x) * 32768.0f, -32768.0f, 32767.0f))
#define Q15_MUL(a, b)   (static_cast<int32_t>((static_cast<int32_t>(a) * static_cast<int32_t>(b)) >> 15))

AudioFilterStateVariable::AudioFilterStateVariable(float sampleRate)
    : low(0), band(0), f(0), q(0), sampleRate_(sampleRate)
{
    setFrequency(1000.0f);   // default 1 kHz
    setResonance(0.707f);    // default Butterworth Q
}

void AudioFilterStateVariable::setFrequency(float frequency)
{
    // Clamp to Nyquist
    if (frequency < 0.0f) frequency = 0.0f;
    if (frequency > sampleRate_ * 0.5f) frequency = sampleRate_ * 0.5f;

    // Compute the normalized frequency coefficient.
    // Using the bilinear transform: f = 2 * sin(π * freq / Fs)
    float omega = static_cast<float>(M_PI) * frequency / sampleRate_;
    float coeff = 2.0f * std::sin(omega);
    f = FLOAT_TO_Q15(coeff);
    recalcCoefficients();
}

void AudioFilterStateVariable::setResonance(float resonance)
{
    // resonance = Q factor; convert to damping coefficient d = 1/Q
    if (resonance < 0.001f) resonance = 0.001f; // avoid division by zero
    float d = 1.0f / resonance;
    // Scale to Q15 (0..32767) – we keep it as a positive value.
    q = FLOAT_TO_Q15(d);
    recalcCoefficients();
}

/* Re‑calculate any derived coefficients – currently a no‑op because we store f and q directly. */
void AudioFilterStateVariable::recalcCoefficients()
{
    // No additional derived coefficients needed for this simple implementation.
}

/**
 * @brief Process one sample.
 *
 * The algorithm follows the classic Chamberlin SVF equations using Q15 arithmetic:
 *
 *   low  = low  + f * band
 *   high = input - low - q * band
 *   band = f * high + band
 *
 * The three outputs are:
 *   low  – low‑pass
 *   band – band‑pass
 *   high – high‑pass
 *
 * All intermediate results are kept in 32‑bit signed integers to avoid overflow.
 */
int16_t AudioFilterStateVariable::update(int16_t input, uint8_t mode)
{
    // Convert input to Q15 (already int16_t, treat as Q15)
    int32_t in = static_cast<int32_t>(input);

    // low  = low  + f * band
    low = low + Q15_MUL(f, static_cast<int16_t>(band));

    // high = in - low - q * band
    int32_t high = in - low - Q15_MUL(q, static_cast<int16_t>(band));

    // band = band + f * high
    band = band + Q15_MUL(f, static_cast<int16_t>(high));

    // Choose output
    int32_t out;
    switch (mode) {
        case 0: // Low‑pass
            out = low;
            break;
        case 1: // Band‑pass
            out = band;
            break;
        case 2: // High‑pass
            out = high;
            break;
        default:
            out = low;
            break;
    }

    // Saturate to int16_t range using __SSAT (signed saturate)
    int16_t result = static_cast<int16_t>(__SSAT(out, 16));
    return result;
}