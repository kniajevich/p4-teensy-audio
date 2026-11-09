#include "output_i2s.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "AudioOutputI2S";

AudioOutputI2S::AudioOutputI2S()
    : AudioStream(2, inputQueueArray), tx_handle(nullptr), is_initialized(false) {
    inputQueueArray[0] = nullptr;
    inputQueueArray[1] = nullptr;
}

AudioOutputI2S::~AudioOutputI2S() {
    if (tx_handle) {
        i2s_channel_disable(tx_handle);
        i2s_del_channel(tx_handle);
    }
}

bool AudioOutputI2S::begin(i2s_port_t port_num, int bclk_io, int ws_io, int dout_io) {
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(port_num, I2S_ROLE_MASTER);
    if (i2s_new_channel(&chan_cfg, &tx_handle, nullptr) != ESP_OK) {
        ESP_LOGE(TAG, "Failed creating I2S channel");
        return false;
    }
    
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG((uint32_t)AUDIO_SAMPLE_RATE_EXACT),
        .slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = (gpio_num_t)bclk_io,
            .ws   = (gpio_num_t)ws_io,
            .dout = (gpio_num_t)dout_io,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = { .mclk_inv = false, .bclk_inv = false, .ws_inv = false },
        },
    };
    
    if (i2s_channel_init_std_mode(tx_handle, &std_cfg) != ESP_OK) return false;
    if (i2s_channel_enable(tx_handle) != ESP_OK) return false;
    
    is_initialized = true;
    return true;
}

void AudioOutputI2S::update() {
    audio_block_t *left = receiveReadOnly(0);
    audio_block_t *right = receiveReadOnly(1);
    
    if (!is_initialized || !tx_handle) {
        if (left) release(left);
        if (right) release(right);
        return;
    }
    
    int16_t interleaved[AUDIO_BLOCK_SAMPLES * 2];
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        interleaved[i * 2]     = left ? left->data[i] : 0;
        interleaved[i * 2 + 1] = right ? right->data[i] : (left ? left->data[i] : 0);
    }
    
    size_t bytes_written = 0;
    i2s_channel_write(tx_handle, interleaved, sizeof(interleaved), &bytes_written, portMAX_DELAY);
    
    if (left) release(left);
    if (right) release(right);
}