#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "AudioStream.h"
#include "synth_sine.h"
#include "output_i2s.h"
#include "filter_variable.h"
#include "mixer.h"

static const char *TAG = "APP_MAIN";

AudioSynthWaveformSine    sineGen;
AudioFilterStateVariable  filter;
AudioMixer4               mixer;
AudioOutputI2S            i2sOut;

// Sine -> Filter -> Mixer (Ch0) -> I2S Stereo
AudioConnection patchCord1(sineGen, 0, filter, 0);
AudioConnection patchCord2(filter, 0, mixer, 0);
// Lowpass output
AudioConnection patchCord3(mixer, 0, i2sOut, 0);
AudioConnection patchCord4(mixer, 0, i2sOut, 1);

void audio_engine_task(void *pvParameters) {
    ESP_LOGI(TAG, "Audio task pinned to core %d", xPortGetCoreID());

    sineGen.frequency(440.0f);
    sineGen.amplitude(0.5f);

    filter.frequency(1200.0f);
    filter.resonance(2.0f);

    mixer.gain(0, 1.0f);

    while (1) {
        AudioStream::update_all();
        vTaskDelay(pdMS_TO_TICKS(3));
    }
}

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Initializing Teensy Audio Engine on ESP32-P4...");

    AudioStream::initialize_memory(64);

    if (!i2sOut.begin(I2S_NUM_0, 10, 11, 12)) {
        ESP_LOGE(TAG, "Failed initializing I2S driver!");
        return;
    }

    xTaskCreatePinnedToCore(
        audio_engine_task,
        "AudioEngine",
        4096,
        nullptr,
        configMAX_PRIORITIES - 2,
        nullptr,
        1
    );

    ESP_LOGI(TAG, "Audio engine successfully initialized.");
}