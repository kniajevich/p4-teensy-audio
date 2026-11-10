#pragma once

#include "AudioStream.h"
#include <stdio.h>

class AudioPlaySdWav : public AudioStream {
public:
    AudioPlaySdWav() : AudioStream(0, NULL), m_file(NULL), m_playing(false) {}
    virtual ~AudioPlaySdWav() { stop(); }

    bool play(const char* filename);
    void stop();
    bool isPlaying() const { return m_playing; }
    virtual void update();

private:
    FILE* m_file;
    bool m_playing;
    uint16_t m_channels;
    uint32_t m_sample_rate;
    uint16_t m_bits_per_sample;
    uint32_t m_data_offset;
};
