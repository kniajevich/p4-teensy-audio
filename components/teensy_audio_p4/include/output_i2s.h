#ifndef OUTPUT_I2S_H
#define OUTPUT_I2S_H

#include "AudioStream.h"
#include "driver/i2s_std.h"

class AudioOutputI2S : public AudioStream {
public:
    AudioOutputI2S();
    virtual ~AudioOutputI2S();
    bool begin(i2s_port_t port_num, int bclk_io, int ws_io, int dout_io);
    virtual void update() override;

private:
    audio_block_t *inputQueueArray[2];
    i2s_chan_handle_t tx_handle;
    bool is_initialized;
};

#endif // OUTPUT_I2S_H
