#include "AudioStream.h"
#include "esp_log.h"

AudioStream *AudioStream::first_update = nullptr;
audio_block_t *AudioStream::memory_pool = nullptr;
uint16_t AudioStream::memory_pool_available_mask[16] = {0};
uint16_t AudioStream::max_blocks = 0;
SemaphoreHandle_t AudioStream::mem_mutex = nullptr;

AudioConnection::AudioConnection(AudioStream &source, AudioStream &destination) 
    : pSrc(&source), pDst(&destination), src_index(0), dest_index(0), next_dest(nullptr), isConnected(false) {
    connect();
}

AudioConnection::AudioConnection(AudioStream &source, unsigned char sourceOutput, AudioStream &destination, unsigned char destinationInput)
    : pSrc(&source), pDst(&destination), src_index(sourceOutput), dest_index(destinationInput), next_dest(nullptr), isConnected(false) {
    connect();
}

AudioConnection::~AudioConnection() {
    disconnect();
}

void AudioConnection::connect(void) {
    if (isConnected) return;
    next_dest = pSrc->destination_list;
    pSrc->destination_list = this;
    isConnected = true;
}

void AudioConnection::disconnect(void) {
    if (!isConnected) return;
    if (pSrc->destination_list == this) {
        pSrc->destination_list = next_dest;
    } else {
        AudioConnection *p = pSrc->destination_list;
        while (p && p->next_dest != this) {
            p = p->next_dest;
        }
        if (p) p->next_dest = next_dest;
    }
    isConnected = false;
}

AudioStream::AudioStream(uint8_t ninput, audio_block_t **iqueue) 
    : num_inputs(ninput), inputQueue(iqueue), destination_list(nullptr) {
    next_stream = first_update;
    first_update = this;
    if (inputQueue) {
        for (uint8_t i = 0; i < num_inputs; i++) {
            inputQueue[i] = nullptr;
        }
    }
}

AudioStream::~AudioStream() {
    if (first_update == this) {
        first_update = next_stream;
    } else {
        AudioStream *p = first_update;
        while (p && p->next_stream != this) {
            p = p->next_stream;
        }
        if (p) p->next_stream = next_stream;
    }
}

void AudioStream::initialize_memory(audio_block_t *data, unsigned int num) {
    if (!mem_mutex) {
        mem_mutex = xSemaphoreCreateMutex();
    }
    memory_pool = data;
    max_blocks = (num > 256) ? 256 : num;
    memset(memory_pool_available_mask, 0xFF, sizeof(memory_pool_available_mask));
    for (unsigned int i = 0; i < max_blocks; i++) {
        memory_pool[i].memory_pool_index = i;
        memory_pool[i].ref_count = 0;
    }
}

audio_block_t *AudioStream::allocate(void) {
    if (!memory_pool || !mem_mutex) return nullptr;
    audio_block_t *block = nullptr;
    if (xSemaphoreTake(mem_mutex, portMAX_DELAY) == pdTRUE) {
        for (unsigned int i = 0; i < max_blocks; i++) {
            int mask_idx = i / 16;
            int bit_idx = i % 16;
            if (memory_pool_available_mask[mask_idx] & (1 << bit_idx)) {
                memory_pool_available_mask[mask_idx] &= ~(1 << bit_idx);
                block = &memory_pool[i];
                block->ref_count = 1;
                break;
            }
        }
        xSemaphoreGive(mem_mutex);
    }
    return block;
}

void AudioStream::release(audio_block_t *block) {
    if (!block || !mem_mutex) return;
    if (xSemaphoreTake(mem_mutex, portMAX_DELAY) == pdTRUE) {
        if (block->ref_count > 1) {
            block->ref_count--;
        } else {
            block->ref_count = 0;
            int idx = block->memory_pool_index;
            int mask_idx = idx / 16;
            int bit_idx = idx % 16;
            memory_pool_available_mask[mask_idx] |= (1 << bit_idx);
        }
        xSemaphoreGive(mem_mutex);
    }
}

void AudioStream::transmit(audio_block_t *block, unsigned int index) {
    for (AudioConnection *c = destination_list; c; c = c->next_dest) {
        if (c->src_index == index && c->pDst->inputQueue) {
            if (c->dest_index < c->pDst->num_inputs) {
                if (!c->pDst->inputQueue[c->dest_index]) {
                    c->pDst->inputQueue[c->dest_index] = block;
                    block->ref_count++;
                }
            }
        }
    }
}

audio_block_t *AudioStream::receiveReadOnly(unsigned int index) {
    if (!inputQueue || index >= num_inputs) return nullptr;
    return inputQueue[index];
}

audio_block_t *AudioStream::receiveWritable(unsigned int index) {
    audio_block_t *in = receiveReadOnly(index);
    if (!in) return nullptr;
    if (in->ref_count == 1) return in;
    audio_block_t *out = allocate();
    if (out) {
        memcpy(out->data, in->data, sizeof(out->data));
    }
    release(in);
    if (inputQueue) inputQueue[index] = out;
    return out;
}

void AudioStream::update_all(void) {
    for (AudioStream *s = first_update; s; s = s->next_stream) {
        s->update();
        if (s->inputQueue) {
            for (uint8_t i = 0; i < s->num_inputs; i++) {
                if (s->inputQueue[i]) {
                    AudioStream::release(s->inputQueue[i]);
                    s->inputQueue[i] = nullptr;
                }
            }
        }
    }
}
