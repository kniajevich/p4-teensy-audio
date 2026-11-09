#include "mixer.h"
#include "arm_math_mock.h"
#include <string.h>

AudioMixer4::AudioMixer4() : AudioStream(4, inputQueueArray) {
    for (int i = 0; i < 4; i++) {
        inputQueueArray[i] = nullptr;
        multiplier[i] = 65536; // 1.0 in 16.16 fixed point
    }
}

void AudioMixer4::gain(unsigned int channel, float gain) {
    if (channel >= 4) return;
    if (gain < 0.0f) gain = 0.0f;
    if (gain > 32767.0f) gain = 32767.0f;
    multiplier[channel] = (int32_t)(gain * 65536.0f);
}

void AudioMixer4::update() {
    audio_block_t *in[4];
    for (int i = 0; i < 4; i++) {
        in[i] = receiveReadOnly(i);
    }
    
    audio_block_t *out = allocate();
    if (!out) {
        for (int i = 0; i < 4; i++) {
            if (in[i]) release(in[i]);
        }
        return;
    }
    
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int64_t sum = 0;
        for (int ch = 0; ch < 4; ch++) {
            if (in[ch]) {
                sum += ((int64_t)in[ch]->data[i] * multiplier[ch]) >> 16;
            }
        }
        out->data[i] = (int16_t)__SSAT((int32_t)sum, 16);
    }
    
    for (int ch = 0; ch < 4; ch++) {
        if (in[ch]) release(in[ch]);
    }
    
    transmit(out, 0);
    release(out);
}