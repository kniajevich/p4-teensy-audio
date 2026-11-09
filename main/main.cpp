#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "AudioStream.h"
#include "synth_sine.h"
#include "effect_envelope.h"
#include "filter_variable.h"
#include "mixer.h"
#include "output_i2s.h"

static const char *TAG = "APP_MAIN";

AudioSynthWaveformSine    sineGen;
AudioEffectEnvelope       envelope;
AudioFilterStateVariable  filter;
AudioMixer4               mixer;
AudioOutputI2S            i2sOut;

// Audio Graph: Sine -> Envelope -> Filter -> Mixer -> I2S Stereo
AudioConnection patchCord1(sineGen, 0, envelope, 0);
AudioConnection patchCord2(envelope, 0, filter, 0);
AudioConnection patchCord3(filter, 0, mixer, 0);
AudioConnection patchCord4(mixer, 0, i2sOut, 0);
AudioConnection patchCord5(mixer, 0, i2sOut, 1);

void audio_engine_task(void *pvParameters) {
    ESP_LOGI(TAG, "Audio task active on Core %d", xPortGetCoreID());

    // Configure Sine Generator
    sineGen.frequency(220.0f); // A3
    sineGen.amplitude(0.8f);

    // Configure Envelope (ADSR)
    envelope.delay(0.0f);
    envelope.attack(15.0f);   // 15ms attack
    envelope.hold(0.0f);      // No hold
    envelope.decay(100.0f);   // 100ms decay
    envelope.sustain(0.4f);   // 40% sustain level
    envelope.release(250.0f); // 250ms release

    // Configure Filter
    filter.frequency(1800.0f);
    filter.resonance(2.2f);

    // Configure Mixer
    mixer.gain(0, 1.0f);

    uint32_t tick = 0;
    const uint32_t NOTE_ON_INTERVAL = 333;  // ~1 second at 3ms tick
    const uint32_t NOTE_OFF_OFFSET = 166;   // ~0.5 second note duration

    while (1) {
        AudioStream::update_all();

        // Trigger note every 1 second
        if (tick % NOTE_ON_INTERVAL == 0) {
            envelope.noteOn();
            ESP_LOGD(TAG, "Note ON. Envelope Level: %.3f", envelope.getCurrentLevel());
        } else if (tick % NOTE_ON_INTERVAL == NOTE_OFF_OFFSET) {
            envelope.noteOff();
            ESP_LOGD(TAG, "Note OFF. Envelope Level: %.3f", envelope.getCurrentLevel());
        }

        tick++;
        vTaskDelay(pdMS_TO_TICKS(3));
    }
}

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting Teensy Audio Synthesizer on ESP32-P4...");

    // Initialize audio stream memory pool
    AudioStream::initialize_memory(64);

    // Initialize I2S Output
    // Pin configuration for ESP32-P4 (example, adjust to your hardware)
    // BCLK=10, WS=11, DOUT=12
    if (!i2sOut.begin(I2S_NUM_0, 10, 11, 12)) {
        ESP_LOGE(TAG, "I2S init failed");
        return;
    }

    // Create audio engine task on Core 1 (high priority)
    xTaskCreatePinnedToCore(
        audio_engine_task,
        "AudioEngine",
        4096,
        nullptr,
        configMAX_PRIORITIES - 2,
        nullptr,
        1
    );

    ESP_LOGI(TAG, "Synthesizer initialized.");
}