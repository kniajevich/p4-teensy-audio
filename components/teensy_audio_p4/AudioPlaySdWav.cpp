#include "AudioPlaySdWav.h"
#include <cstring>

bool AudioPlaySdWav::play(const char* filename) {
    stop();
    m_file = fopen(filename, "rb");
    if (!m_file) return false;

    char header[44];
    if (fread(header, 1, 44, m_file) < 44) { stop(); return false; }
    if (memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVE", 4) != 0) { stop(); return false; }

    m_channels = header[22] | (header[23] << 8);
    m_sample_rate = header[24] | (header[25] << 8) | (header[26] << 16) | (header[27] << 24);
    m_bits_per_sample = header[34] | (header[35] << 8);
    m_data_offset = 44;
    m_playing = true;
    return true;
}

void AudioPlaySdWav::stop() {
    m_playing = false;
    if (m_file) { fclose(m_file); m_file = NULL; }
}

void AudioPlaySdWav::update() {
    if (!m_playing || !m_file) return;
    audio_block_t* outL = allocate();
    audio_block_t* outR = allocate();
    if (!outL && !outR) return;

    int16_t raw_buffer[AUDIO_BLOCK_SAMPLES * 2];
    size_t samples_to_read = AUDIO_BLOCK_SAMPLES * m_channels;
    size_t read_bytes = fread(raw_buffer, sizeof(int16_t), samples_to_read, m_file);

    if (read_bytes < samples_to_read) {
        stop();
        if (outL) release(outL);
        if (outR) release(outR);
        return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        if (m_channels == 1) {
            int16_t s = raw_buffer[i];
            if (outL) outL->data[i] = s;
            if (outR) outR->data[i] = s;
        } else {
            if (outL) outL->data[i] = raw_buffer[i * 2];
            if (outR) outR->data[i] = raw_buffer[i * 2 + 1];
        }
    }
    if (outL) { transmit(outL, 0); release(outL); }
    if (outR) { transmit(outR, 1); release(outR); }
}
