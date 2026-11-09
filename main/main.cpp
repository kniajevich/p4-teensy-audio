#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "audio_output_i2s.h"
#include "audio_synth_waveform_sine.h"
#include "filter_variable.h"
#include "mixer.h"

/* Constants */
constexpr float SAMPLE_RATE = 44100.0f;
constexpr uint32_t BUFFER_SIZE = 256;

/* Global objects */
static AudioSynthWaveformSine   synth;
static AudioFilterStateVariable filter;
static AudioMixer4             mixer;
static AudioOutputI2S          i2sOut;

/* Simple audio task */
extern "C" void app_main(void)
{
    // Configure the sine wave generator
    synth.begin(440.0f);               // A4 = 440 Hz

    // Configure the state‑variable filter (Low‑pass, 1200 Hz, Q = 2.0)
    filter.setFrequency(1200.0f);
    filter.setResonance(2.0f);         // Q factor = 2.0

    // Mixer: use only channel 0, mute others
    mixer.gain(0, 32767.0f);           // full level
    mixer.gain(1, 0.0f);
    mixer.gain(2, 0.0f);
    mixer.gain(3, 0.0f);

    // Initialise I2S output (stereo, 16‑bit)
    i2sOut.begin(SAMPLE_RATE, 2);

    int16_t buffer[BUFFER_SIZE];

    while (true) {
        // Fill buffer with processed samples
        for (uint32_t i = 0; i < BUFFER_SIZE; ++i) {
            int16_t raw   = synth.update();                     // sine wave
            int16_t filt  = filter.update(raw, 0);              // low‑pass output
            int16_t mixed = mixer.mix(filt, 0, 0, 0);           // only channel 0 active
            buffer[i] = mixed;
        }

        // Send to I2S (stereo – duplicate sample for left/right)
        i2sOut.write(buffer, BUFFER_SIZE);
    }
}