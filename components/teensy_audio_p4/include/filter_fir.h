#pragma once

#include "AudioStream.h"
#include <string.h>

class AudioFilterFIR : public AudioStream {
public:
    AudioFilterFIR() : AudioStream(1, inputQueueArray), coeff(nullptr), num_coeffs(0), state_idx(0) {
        memset(state, 0, sizeof(state));
    }
    void begin(const int16_t *coeffs, short n) {
        coeff = coeffs;
        num_coeffs = (n > 256) ? 256 : (n < 0 ? 0 : n);
    }
    void end() { coeff = nullptr; num_coeffs = 0; }
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[1];
    const int16_t *coeff;
    short num_coeffs;
    int16_t state[256];
    short state_idx;
};
