#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "Audio.h"

static const char *TAG = "P4_TEENSY_AUDIO";

// DSP Chain Objects
AudioSynthWaveform          lfo;
AudioSynthWaveformModulated carrier;
AudioSynthNoisePink         pink_noise;
AudioFilterBiquad           filter;
AudioEffectDelay            delay1;
AudioMixer8                 mixer;
AudioOutputI2S              i2s_out;

// Audio Connections (patch cord logic)
AudioConnection patchCord1(lfo, 0, carrier, 0);          // LFO -> FM input
AudioConnection patchCord2(carrier, 0, filter, 0);       // Carrier -> Biquad
AudioConnection patchCord3(filter, 0, mixer, 0);         // Filtered synth -> Mix CH0
AudioConnection patchCord4(filter, 0, delay1, 0);        // Filtered synth -> Delay
AudioConnection patchCord5(delay1, 0, mixer, 1);         // Delay tap -> Mix CH1
AudioConnection patchCord6(pink_noise, 0, mixer, 2);     // Pink noise -> Mix CH2
AudioConnection patchCord7(mixer, 0, i2s_out, 0);        // Main mix L
AudioConnection patchCord8(mixer, 0, i2s_out, 1);        // Main mix R

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Initializing Teensy Audio Engine on ESP32-P4...");

    AudioMemory(32);

    // 1. Setup LFO (modulator)
    lfo.begin(WAVEFORM_TRIANGLE);
    lfo.frequency(4.0f);
    lfo.amplitude(0.35f);

    // 2. Setup FM Carrier
    carrier.begin(WAVEFORM_SAWTOOTH);
    carrier.frequency(220.0f); // A3 note
    carrier.amplitude(0.5f);
    carrier.frequencyModulation(1.5f);

    // 3. Setup Pink Noise layer
    pink_noise.amplitude(0.04f);

    // 4. Setup 2-stage Lowpass Filter (Biquad)
    filter.setLowpass(0, 1200.0f, 2.5f);
    filter.setLowpass(1, 1400.0f, 1.5f);

    // 5. Setup Ping-pong style single Delay
    delay1.delay(0, 280.0f); // 280ms delay

    // 6. Balance Mixer Channels
    mixer.gain(0, 0.7f); // Dry synth
    mixer.gain(1, 0.4f); // Delay return
    mixer.gain(2, 0.15f); // Subtle noise texture

    // 7. Start I2S Engine
    i2s_out.begin();

    ESP_LOGI(TAG, "Engine running. Cycling base notes...");

    float melody[] = { 220.0f, 246.94f, 261.63f, 293.66f, 329.63f, 392.00f };
    int note_idx = 0;

    while (1) {
        carrier.frequency(melody[note_idx]);
        note_idx = (note_idx + 1) % 6;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
