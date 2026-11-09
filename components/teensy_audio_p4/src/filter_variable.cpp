/*
 *  AudioFilterStateVariable.cpp
 *  Implementation of a State‑Variable Filter (SVF) for the Teensy‑Audio‑P4 library.
 *
 *  The filter provides three simultaneous outputs:
 *      0 – Low‑pass
 *      1 – Band‑pass
 *      2 – High‑pass
 *
 *  The algorithm follows the classic Chamberlin SVF formulation and works
 *  entirely in fixed‑point Q15 arithmetic to keep the processing load low on
 *  the ESP32‑P4 (RISC‑V) core.
 *
 *  All arithmetic is saturated to 16‑bit signed range using the __SSAT
 *  intrinsic provided by the ARM CMSIS core (available in ESP‑IDF as well).
 *
 *  Author:  OpenAI ChatGPT
 *  License: MIT
 */

#include "filter_variable.h"
#include "arm_math_mock.h"          // Provides __SSAT for the mock build environment
#include <math.h>                   // sinf, M_PI

/* -------------------------------------------------------------------------- */
AudioFilterStateVariable::AudioFilterStateVariable()
    : AudioStream(1, inputQueueArray),   // one input, zero‑copy queue
      setting_f(0),
      setting_q(0),
      state_lowpass(0),
      state_bandpass(0)
{
    // Initialise the queue pointer (required by AudioStream base class)
    inputQueueArray[0] = nullptr;

    // Default filter parameters – 1 kHz low‑pass, Q = 1 (no resonance)
    setFrequency(1000.0f);
    setResonance(1.0f);
}

/* -------------------------------------------------------------------------- */
void AudioFilterStateVariable::setFrequency(float freq)
{
    // Clamp to a useful audible range (20 Hz … 0.45 × Fs)
    if (freq < 20.0f)      freq = 20.0f;
    if (freq > AUDIO_SAMPLE_RATE_EXACT * 0.45f)
        freq = AUDIO_SAMPLE_RATE_EXACT * 0.45f;

    // Compute the normalized angular frequency:
    //   f = 2 * sin(π * freq / Fs)   (range 0 … 2)
    // Convert to Q15 (0 … 32767) for integer arithmetic.
    float w = 2.0f * sinf((float)M_PI * freq / AUDIO_SAMPLE_RATE_EXACT);
    setting_f = static_cast<int32_t>(w * 32768.0f);   // Q15 format
}

/* -------------------------------------------------------------------------- */
void AudioFilterStateVariable::setResonance(float q)
{
    // Clamp Q to a practical range (0.7 … 5.0)
    if (q < 0.7f)  q = 0.7f;
    if (q > 5.0f)  q = 5.0f;

    // Damping factor = 1 / Q, also stored in Q15.
    float damp = 1.0f / q;
    setting_q = static_cast<int32_t>(damp * 32768.0f);   // Q15 format
}

/* -------------------------------------------------------------------------- */
void AudioFilterStateVariable::update()
{
    audio_block_t *in = receiveReadOnly(0);
    if (!in) {
        return;                     // No input – nothing to do
    }

    // Allocate output blocks for the three filter outputs.
    audio_block_t *out_lp = allocate();   // low‑pass
    audio_block_t *out_bp = allocate();   // band‑pass
    audio_block_t *out_hp = allocate();   // high‑pass

    // Process the block sample‑by‑sample.
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i) {
        // Input sample is 16‑bit signed; promote to 32‑bit for the calculations.
        int32_t input = static_cast<int32_t>(in->data[i]);

        // ------------------------------------------------------------------
        // Chamberlin SVF core equations (all values are Q15 fixed‑point):
        //   hp = input - low - q * band
        //   band += f * hp
        //   low  += f * band
        // ------------------------------------------------------------------

        // High‑pass output (still Q15)
        int32_t hp = input
                     - (state_lowpass >> 15)                     // low in Q15
                     - ((state_bandpass * setting_q) >> 15);    // q*band in Q15
        hp = __SSAT(hp, 16);   // Saturate to signed 16‑bit range

        // Band‑pass state update
        state_bandpass += (hp * setting_f);   // Q15 * Q15 → Q30, keep as Q30
        int32_t bp = state_bandpass >> 15;    // Convert back to Q15
        bp = __SSAT(bp, 16);

        // Low‑pass state update
        state_lowpass += (bp * setting_f);    // Q15 * Q15 → Q30
        int32_t lp = state_lowpass >> 15;     // Back to Q15
        lp = __SSAT(lp, 16);

        // Store results (cast to 16‑bit signed)
        if (out_lp) out_lp->data[i] = static_cast<int16_t>(lp);
        if (out_bp) out_bp->data[i] = static_cast<int16_t>(bp);
        if (out_hp) out_hp->data[i] = static_cast<int16_t>(hp);
    }

    // Release the input block now that we are done with it.
    release(in);

    // Transmit each non‑null output block on its respective channel and free it.
    if (out_lp) {
        transmit(out_lp, 0);
        release(out_lp);
    }
    if (out_bp) {
        transmit(out_bp, 1);
        release(out_bp);
    }
    if (out_hp) {
        transmit(out_hp, 2);
        release(out_hp);
    }
}

/* -------------------------------------------------------------------------- */