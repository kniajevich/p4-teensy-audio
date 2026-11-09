#include "input_i2s.h"
#include "esp_log.h"

static const char *TAG = "AudioInputI2S";

AudioInputI2S::~AudioInputI2S() {
    stop();
}

bool AudioInputI2S::begin(i2s_port_t port_num, int bclk_io, int ws_io, int din_io) {
    if (is_initialized) return true;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(port_num, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = 6;
    chan_cfg.dma_frame_num = AUDIO_BLOCK_SAMPLES;

    if (i2s_new_channel(&chan_cfg, nullptr, &rx_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2S RX channel");
        return false;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG((uint32_t)AUDIO_SAMPLE_RATE_EXACT),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = (gpio_num_t)bclk_io,
            .ws   = (gpio_num_t)ws_io,
            .dout = I2S_GPIO_UNUSED,
            .din  = (gpio_num_t)din_io,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };

    if (i2s_channel_init_std_mode(rx_handle, &std_cfg) != ESP_OK) return false;
    if (i2s_channel_enable(rx_handle) != ESP_OK) return false;

    is_initialized = true;
    return true;
}

void AudioInputI2S::stop() {
    if (is_initialized && rx_handle) {
        i2s_channel_disable(rx_handle);
        i2s_del_channel(rx_handle);
        rx_handle = nullptr;
        is_initialized = false;
    }
}

void AudioInputI2S::update(void) {
    if (!is_initialized || !rx_handle) return;

    audio_block_t *out_l = allocate();
    audio_block_t *out_r = allocate();
    if (!out_l || !out_r) {
        if (out_l) release(out_l);
        if (out_r) release(out_r);
        return;
    }

    int16_t interleaved[AUDIO_BLOCK_SAMPLES * 2];
    size_t bytes_read = 0;
    if (i2s_channel_read(rx_handle, interleaved, sizeof(interleaved), &bytes_read, 0) == ESP_OK && bytes_read == sizeof(interleaved)) {
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            out_l->data[i] = interleaved[i * 2 + 0];
            out_r->data[i] = interleaved[i * 2 + 1];
        }
    } else {
        memset(out_l->data, 0, sizeof(out_l->data));
        memset(out_r->data, 0, sizeof(out_r->data));
    }

    transmit(out_l, 0);
    transmit(out_r, 1);
    release(out_l);
    release(out_r);
}
