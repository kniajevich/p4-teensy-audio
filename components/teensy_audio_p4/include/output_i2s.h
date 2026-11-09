#pragma once

#include "AudioStream.h"
#include "driver/i2s_std.h"

class AudioOutputI2S : public AudioStream {
public:
    AudioOutputI2S() : AudioStream(2, inputQueueArray), tx_handle(nullptr), is_initialized(false) {}
    virtual ~AudioOutputI2S();
    
    bool begin(i2s_port_t port_num = I2S_NUM_0, int bclk_io = 12, int ws_io = 13, int dout_io = 11);
    void stop();
    virtual void update(void) override;

private:
    audio_block_t *inputQueueArray[2];
    i2s_chan_handle_t tx_handle;
    bool is_initialized;
};
