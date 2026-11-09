#pragma once

#include "AudioStream.h"
#include "driver/i2s_std.h"

class AudioInputI2S : public AudioStream {
public:
    AudioInputI2S() : AudioStream(0, nullptr), rx_handle(nullptr), is_initialized(false) {}
    virtual ~AudioInputI2S();
    
    bool begin(i2s_port_t port_num = I2S_NUM_1, int bclk_io = 14, int ws_io = 15, int din_io = 10);
    void stop();
    virtual void update(void) override;

private:
    i2s_chan_handle_t rx_handle;
    bool is_initialized;
};
