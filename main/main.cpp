#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "AudioStream.h"
#include "synth_sine.h"
#include "output_i2s.h"

static const char *TAG = "APP_MAIN";

AudioSynthWaveformSine sineGen;
AudioOutputI2S         i2sOut;
AudioConnection        patchCord1(sineGen, 0, i2sOut, 0);
AudioConnection        patchCord2(sineGen, 0, i2sOut, 1);

void audio_engine_task(void *pvParameters) {
    ESP_LOGI(TAG, "Audio task pinned to core %d", xPortGetCoreID());
    
    // Set 440 Hz standard pitch
    sineGen.frequency(440.0f);
    sineGen.amplitude(0.5f);

    while (1) {
        AudioStream::update_all();
        // 128 samples at 44100Hz = ~2.9ms period
        vTaskDelay(pdMS_TO_TICKS(3));
    }
}

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Initializing Teensy Audio Engine for ESP32-P4...");

    AudioStream::initialize_memory(64);

    // Default ESP32-P4 GPIO pins for I2S: BCLK=10, WS=11, DOUT=12
    if (!i2sOut.begin(I2S_NUM_0, 10, 11, 12)) {
        ESP_LOGE(TAG, "Failed initializing I2S driver!");
        return;
    }

    // FreeRTOS task strictly pinned to Core 1
    xTaskCreatePinnedToCore(
        audio_engine_task,
        "AudioEngine",
        4096,
        nullptr,
        configMAX_PRIORITIES - 2,
        nullptr,
        1
    );

    ESP_LOGI(TAG, "Audio engine running.");
}
