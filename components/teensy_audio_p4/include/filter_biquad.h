#pragma once

#include "AudioStream.h"
#include <cmath>

class AudioFilterBiquad : public AudioStream {
public:
    AudioFilterBiquad() : AudioStream(1, inputQueueArray) {
        for (int i = 0; i < 4; i++) {
            coef[i * 5 + 0] = 1073741824; // b0 = 1.0 (Q30)
            coef[i * 5 + 1] = 0;
            coef[i * 5 + 2] = 0;
            coef[i * 5 + 3] = 0;
            coef[i * 5 + 4] = 0;
        }
    }

    void setLowpass(uint32_t stage, float frequency, float q = 0.7071f);
    void setHighpass(uint32_t stage, float frequency, float q = 0.7071f);
    void setBandpass(uint32_t stage, float frequency, float q = 1.0f);
    void setCoefficients(uint32_t stage, const int32_t *coefficients);
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[1];
    int32_t coef[20]; // 4 stages * 5 coeffs (b0, b1, b2, a1, a2) in Q30
    int32_t state[16] = {0}; // 4 stages * 4 state vars (x1, x2, y1, y2)
};
