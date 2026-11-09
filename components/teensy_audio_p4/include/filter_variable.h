#ifndef FILTER_VARIABLE_H
#define FILTER_VARIABLE_H

#include "AudioStream.h"
#include <stdint.h>

/**
 * @brief State‑variable filter (low‑pass / band‑pass / high‑pass).
 *
 * Provides the standard Teensy Audio API setters (`setFrequency`,
 * `setResonance`) together with the API‑compatible aliases
 * `frequency()` and `resonance()`.
 */
class AudioFilterStateVariable : public AudioStream {
public:
    AudioFilterStateVariable();
    virtual ~AudioFilterStateVariable() {}

    /** Set the cutoff frequency in Hz. */
    void setFrequency(float freq);

    /** Set the resonance (Q factor). */
    void setResonance(float q);

    /** Alias for setFrequency – required by the Teensy API. */
    void frequency(float freq) { setFrequency(freq); }

    /** Alias for setResonance – required by the Teensy API. */
    void resonance(float q) { setResonance(q); }

    /** @brief Called by the audio engine each processing block. */
    virtual void update() override;

private:
    /** Queue of incoming audio blocks (single input). */
    audio_block_t *inputQueueArray[1];

    /** Internal fixed‑point representations of the parameters. */
    int32_t setting_f;      // Frequency (Q1.31)
    int32_t setting_q;      // Resonance (Q1.31)

    /** Filter state variables (low‑pass and band‑pass). */
    int32_t state_lowpass;
    int32_t state_bandpass;
};

#endif // FILTER_VARIABLE_H