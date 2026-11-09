#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <stdint.h>
#include <stddef.h>
#include "esp_heap_caps.h"

#define AUDIO_BLOCK_SAMPLES 128
#define AUDIO_SAMPLE_RATE_EXACT 44100.0f

struct audio_block_t {
    uint8_t ref_count;
    uint8_t reserved;
    int16_t data[AUDIO_BLOCK_SAMPLES];
};

class AudioStream;

class AudioConnection {
public:
    AudioConnection(AudioStream &source, AudioStream &destination);
    AudioConnection(AudioStream &source, unsigned char sourceOutput,
                    AudioStream &destination, unsigned char destinationInput);
    ~AudioConnection();
    void connect();
    void disconnect();

protected:
    AudioStream &src;
    AudioStream &dst;
    unsigned char src_index;
    unsigned char dest_index;
    AudioConnection *next_dest;
    bool isConnected;
    friend class AudioStream;
};

class AudioStream {
public:
    AudioStream(unsigned char ninput, audio_block_t **iqueue);
    virtual ~AudioStream() {}
    static void initialize_memory(size_t numblocks);
    static void update_all();
    virtual void update() = 0;

protected:
    static audio_block_t *allocate();
    static void release(audio_block_t *block);
    void transmit(audio_block_t *block, unsigned char index = 0);
    audio_block_t *receiveReadOnly(unsigned char index = 0);
    audio_block_t *receiveWritable(unsigned char index = 0);

    unsigned char num_inputs;
    audio_block_t **inputQueue;

private:
    AudioConnection *destination_list;
    AudioStream *next_stream;
    static AudioStream *first_update;
    static audio_block_t *memory_pool;
    static size_t memory_pool_size;
    static bool memory_initialized;

    friend class AudioConnection;
};

#endif // AUDIOSTREAM_H
