#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define AUDIO_BLOCK_SAMPLES 128
#define AUDIO_SAMPLE_RATE_EXACT 44100.0f

typedef struct audio_block_struct {
    uint8_t ref_count;
    uint8_t reserved1;
    uint16_t memory_pool_index;
    int16_t data[AUDIO_BLOCK_SAMPLES];
} audio_block_t;

class AudioStream;

class AudioConnection {
public:
    AudioConnection(AudioStream &source, AudioStream &destination);
    AudioConnection(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput);
    ~AudioConnection();
    void disconnect(void);
    void connect(void);

    AudioStream *pSrc;
    AudioStream *pDst;
    unsigned char src_index;
    unsigned char dest_index;
    AudioConnection *next_dest;
    bool isConnected;
};

class AudioStream {
    friend class AudioConnection;
public:
    AudioStream(uint8_t ninput, audio_block_t **iqueue);
    virtual ~AudioStream();
    virtual void update(void) = 0;

    static void initialize_memory(audio_block_t *data, unsigned int num);
    static void update_all(void);

protected:
    audio_block_t *receiveReadOnly(unsigned int index = 0);
    audio_block_t *receiveWritable(unsigned int index = 0);
    void transmit(audio_block_t *block, unsigned int index = 0);
    static void release(audio_block_t *block);
    static audio_block_t *allocate(void);

    uint8_t num_inputs;
    audio_block_t **inputQueue;
    AudioConnection *destination_list;
    AudioStream *next_stream;
    static AudioStream *first_update;

private:
    static audio_block_t *memory_pool;
    static uint16_t memory_pool_available_mask[16];
    static uint16_t max_blocks;
    static SemaphoreHandle_t mem_mutex;
};

inline void AudioMemory(unsigned int maxblocks) {
    static audio_block_t pool[256];
    if (maxblocks > 256) maxblocks = 256;
    AudioStream::initialize_memory(pool, maxblocks);
}
