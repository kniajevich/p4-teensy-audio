#include "AudioStream.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"

static const char *TAG = "AudioStream";

AudioStream *AudioStream::first_update = nullptr;
audio_block_t *AudioStream::memory_pool = nullptr;
size_t AudioStream::memory_pool_size = 0;
bool AudioStream::memory_initialized = false;

AudioConnection::AudioConnection(AudioStream &source, AudioStream &destination)
    : src(source), dst(destination), src_index(0), dest_index(0), next_dest(nullptr), isConnected(false) {
    connect();
}

AudioConnection::AudioConnection(AudioStream &source, unsigned char sourceOutput,
                                 AudioStream &destination, unsigned char destinationInput)
    : src(source), dst(destination), src_index(sourceOutput), dest_index(destinationInput), next_dest(nullptr), isConnected(false) {
    connect();
}

AudioConnection::~AudioConnection() {
    disconnect();
}

void AudioConnection::connect() {
    if (isConnected) return;
    next_dest = src.destination_list;
    src.destination_list = this;
    isConnected = true;
}

void AudioConnection::disconnect() {
    if (!isConnected) return;
    AudioConnection **p = &src.destination_list;
    while (*p) {
        if (*p == this) {
            *p = next_dest;
            break;
        }
        p = &((*p)->next_dest);
    }
    isConnected = false;
}

AudioStream::AudioStream(unsigned char ninput, audio_block_t **iqueue)
    : num_inputs(ninput), inputQueue(iqueue), destination_list(nullptr) {
    next_stream = first_update;
    first_update = this;
}

void AudioStream::initialize_memory(size_t numblocks) {
    if (memory_initialized) return;
    memory_pool_size = numblocks;
    
    // SPIRAM alloc with fallback for ESP32-P4
    memory_pool = (audio_block_t *)heap_caps_calloc(numblocks, sizeof(audio_block_t), MALLOC_CAP_SPIRAM);
    if (!memory_pool) {
        ESP_LOGW(TAG, "Failed SPIRAM alloc, fallback to internal RAM");
        memory_pool = (audio_block_t *)calloc(numblocks, sizeof(audio_block_t));
    }
    memory_initialized = (memory_pool != nullptr);
}

audio_block_t *AudioStream::allocate() {
    if (!memory_pool) return nullptr;
    for (size_t i = 0; i < memory_pool_size; i++) {
        if (memory_pool[i].ref_count == 0) {
            memory_pool[i].ref_count = 1;
            return &memory_pool[i];
        }
    }
    return nullptr;
}

void AudioStream::release(audio_block_t *block) {
    if (block && block->ref_count > 0) {
        block->ref_count--;
    }
}

void AudioStream::transmit(audio_block_t *block, unsigned char index) {
    for (AudioConnection *c = destination_list; c; c = c->next_dest) {
        if (c->src_index == index) {
            if (c->dst.inputQueue && c->dest_index < c->dst.num_inputs) {
                if (!c->dst.inputQueue[c->dest_index]) {
                    c->dst.inputQueue[c->dest_index] = block;
                    block->ref_count++;
                }
            }
        }
    }
}

audio_block_t *AudioStream::receiveReadOnly(unsigned char index) {
    if (index >= num_inputs || !inputQueue) return nullptr;
    return inputQueue[index];
}

audio_block_t *AudioStream::receiveWritable(unsigned char index) {
    audio_block_t *in = receiveReadOnly(index);
    if (!in) return nullptr;
    if (in->ref_count == 1) return in;
    audio_block_t *out = allocate();
    if (out) {
        memcpy(out->data, in->data, sizeof(out->data));
    }
    release(in);
    return out;
}

void AudioStream::update_all() {
    for (AudioStream *s = first_update; s; s = s->next_stream) {
        s->update();
        if (s->inputQueue) {
            for (unsigned char i = 0; i < s->num_inputs; i++) {
                if (s->inputQueue[i]) {
                    release(s->inputQueue[i]);
                    s->inputQueue[i] = nullptr;
                }
            }
        }
    }
}
