# Teensy Audio Library for ESP32-P4

[![ESP-IDF Build CI](https://github.com/kniajevich/p4-teensy-audio/actions/workflows/build.yml/badge.svg)](https://github.com/kniajevich/p4-teensy-audio/actions)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)

High-performance audio processing engine ported from the Teensy Audio Library (`AudioStream` architecture) to Espressif **ESP32-P4** (RISC-V dual-core) running on **ESP-IDF v5.3+**.

---

## Author & Maintainer

* **Port Architect & Lead Developer:** Alex Ovchinnikov ([@kniajevich](https://github.com/kniajevich))
* **Original Teensy Audio Library:** Paul Stoffregen ([PJRC.COM, LLC](https://www.pjrc.com/teensy/td_libs_Audio.html))

---

## Architecture Overview

The library retains the asynchronous block-processing paradigm of PJRC's Teensy Audio Library while leveraging native FreeRTOS primitives and ESP-IDF standard I2S DMA drivers.

- **Block Size (`AUDIO_BLOCK_SAMPLES`):** 128 samples per update cycle (~2.9 ms latency at 44.1 kHz).
- **Sample Format:** 16-bit signed PCM, mono blocks routed into stereo DMA streams.
- **Sample Rate:** 44.1 kHz (configurable via exact clock divider settings).
- **Memory Model:** Zero-copy audio block buffer pool with atomic FreeRTOS mutex synchronization.

---

## Supported Modules

| Category | Classes |
| :--- | :--- |
| **Core Engine** | `AudioStream`, `AudioConnection`, `AudioMemory` |
| **I/O Drivers** | `AudioOutputI2S`, `AudioInputI2S` |
| **Synthesis** | `AudioSynthWaveformSine`, `AudioSynthNoiseWhite`, `AudioSynthNoisePink`, `AudioSynthWaveform`, `AudioSynthWaveformModulated` |
| **Filters** | `AudioFilterStateVariable`, `AudioFilterBiquad` (up to 4 cascaded stages) |
| **Routing & Mixing** | `AudioMixer4`, `AudioMixer8`, `AudioAmplifier`, `AudioEffectPan`, `AudioEffectPassthrough` |
| **Dynamics & Effects** | `AudioEffectEnvelope` (ADSR), `AudioEffectDelay`, `AudioEffectBitcrusher`, `AudioEffectMultiply` (Ring Mod), `AudioEffectWaveshaper` |
| **Analysis** | `AudioAnalyzePeak`, `AudioAnalyzeRMS`, `AudioAnalyzeFFT1024` |

---

## Default Hardware Pinout (ESP32-P4)

The built-in I2S driver defaults to standard ESP32-P4 DevKit GPIOs (Master mode, Philips standard):

| Signal | ESP32-P4 GPIO | Description |
| :--- | :--- | :--- |
| **BCLK** | GPIO 12 | Bit Clock |
| **WS / LRCK** | GPIO 13 | Word Select (Left/Right clock) |
| **DOUT** | GPIO 11 | Serial Audio Data Output (to DAC) |
| **DIN** | GPIO 10 | Serial Audio Data Input (from ADC) |

Custom pin mappings can be passed directly to `AudioOutputI2S::begin(...)` and `AudioInputI2S::begin(...)`.

---

## Quick Start

```cpp
#include "Audio.h"

AudioSynthWaveformSine sine1;
AudioOutputI2S         i2s_out;
AudioConnection        patchCord1(sine1, 0, i2s_out, 0);
AudioConnection        patchCord2(sine1, 0, i2s_out, 1);

extern "C" void app_main(void) {
    AudioMemory(16);

    sine1.frequency(440.0f);
    sine1.amplitude(0.5f);

    i2s_out.begin();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

---

## Build & Verification

The repository includes a continuous integration workflow (`.github/workflows/build.yml`) that verifies every commit against `espressif/idf:release-v5.3` container.

To build locally:
```bash
idf.py set-target esp32p4
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

---

## License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.
See the [LICENSE](LICENSE) file for complete terms.

Upstream Teensy Audio Library codebase is Copyright (c) PJRC.COM, LLC and licensed under the MIT License.
