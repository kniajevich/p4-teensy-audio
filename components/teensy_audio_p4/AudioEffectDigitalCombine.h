#pragma once

#include "AudioStream.h"

enum AudioDigitalCombineMode {
    COMBINE_AND = 0,
    COMBINE_OR  = 1,
    COMBINE_XOR = 2,
    COMBINE_MOD = 3
};

class AudioEffectDigitalCombine : public AudioStream {
public:
    AudioEffectDigitalCombine() : AudioStream(2, m_input_queue), m_mode(COMBINE_XOR) {}
    void setMode(AudioDigitalCombineMode mode) { m_mode = mode; }
    virtual void update();

private:
    audio_block_t* m_input_queue[2];
    AudioDigitalCombineMode m_mode;
};
