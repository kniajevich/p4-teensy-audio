# Teensy Audio Library for ESP32-P4

[![ESP-IDF Build CI](https://github.com/kniajevich/p4-teensy-audio/actions/workflows/build.yml/badge.svg)](https://github.com/kniajevich/p4-teensy-audio/actions)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)

High-performance audio processing engine ported from the Teensy Audio Library (`AudioStream` architecture) to Espressif **ESP32-P4** (RISC-V dual-core) running on **ESP-IDF v5.3+**.

---

## Acknowledgments & Upstream Attribution

This project is an independent port and architectural adaptation of the renowned **Teensy Audio Library**, originally created by **Paul Stoffregen ([PJRC.COM, LLC](https://www.pjrc.com/teensy/td_libs_Audio.html))**.

* **Original Architecture & DSP Concepts:** Copyright (c) Paul Stoffregen / PJRC.COM, LLC (distributed under the permissive MIT license).
* **ESP32-P4 Port, FreeRTOS Integration & Modernization:** Alex Ovchinnikov ([@kniajevich](https://github.com/kniajevich)).

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
| **Core Engine** | `AudioStream`, `AudioConnection`, `AudioMemory`, `AudioPlayQueue`, `AudioRecordQueue` |
| **I/O Drivers** | `AudioOutputI2S`, `AudioInputI2S` |
| **Synthesis** | `AudioSynthWaveformSine`, `AudioSynthNoiseWhite`, `AudioSynthNoisePink`, `AudioSynthWaveform`, `AudioSynthWaveformModulated`, `AudioSynthKarplusStrong`, `AudioSynthToneSweep`, `AudioSynthSimpleDrum` |
| **Filters** | `AudioFilterStateVariable`, `AudioFilterBiquad` (up to 4 cascaded stages), `AudioFilterFIR` |
| **Routing & Level** | `AudioMixer4`, `AudioMixer8`, `AudioAmplifier`, `AudioEffectPan`, `AudioEffectPassthrough`, `AudioEffectMidSide`, `AudioEffectFade` |
| **Dynamics & Effects** | `AudioEffectEnvelope` (ADSR), `AudioEffectDelay`, `AudioEffectBitcrusher`, `AudioEffectMultiply` (Ring Mod), `AudioEffectWaveshaper`, `AudioEffectChorus` (with Fibonacci Spread), `AudioEffectFlange`, `AudioEffectFreeverb` (with Golden Ratio tuning), `AudioEffectGranular` (Pitch Shift & Freeze) |
| **Analysis & Pitch Tracking** | `AudioAnalyzePeak`, `AudioAnalyzeRMS`, `AudioAnalyzeFFT1024`, `AudioAnalyzeFFT256`, `AudioAnalyzeToneDetect`, `AudioAnalyzeNoteFreq` (YIN Guitar Tuner) |

---

## Installation & Integration Guide

### Method 1: Using as an ESP-IDF Component (Recommended)

Clone this repository directly into your project's `components/` directory:
```bash
cd my_esp32p4_project/components
git clone [https://github.com/kniajevich/p4-teensy-audio.git](https://github.com/kniajevich/p4-teensy-audio.git) teensy_audio_p4
```

Ensure your `CMakeLists.txt` in `main/` declares dependency on the component:
```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES teensy_audio_p4
)
```

### Method 2: Building Standalone Firmware

```bash
git clone [https://github.com/kniajevich/p4-teensy-audio.git](https://github.com/kniajevich/p4-teensy-audio.git)
cd p4-teensy-audio

# Configure target for ESP32-P4
idf.py set-target esp32p4

# Build the project
idf.py build

# Flash to hardware and monitor serial output
idf.py -p /dev/ttyUSB0 flash monitor
```

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

## Quick Start (Guitar Tuner Example)

```cpp
#include "Audio.h"

AudioInputI2S        i2s_in;
AudioAnalyzeNoteFreq tuner;
AudioConnection      cord(i2s_in, 0, tuner, 0);

extern "C" void app_main(void) {
    AudioMemory(16);
    tuner.begin(0.15f); // 0.15 threshold for clean string detection
    i2s_in.begin();

    while (1) {
        if (tuner.available()) {
            float freq = tuner.read();
            float prob = tuner.probability();
            if (prob > 0.85f) {
                printf("Pitch: %.2f Hz (Confidence: %.0f%%)\n", freq, prob * 100.0f);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

---

## Dual Licensing & Commercial Inquiries

This project is distributed under a **Dual-Licensing model**:

1. **Open Source (GPLv3):** Free for non-commercial use, research, education, and open-source projects. In accordance with the GNU General Public License v3.0, any derivative work or firmware incorporating this code must also be fully open-sourced under the GPLv3 license.
2. **Commercial / Closed-Source License:** If you wish to incorporate this library into commercial hardware devices, proprietary audio gear, or closed-source firmware where GPLv3 copyleft terms cannot be met, you must obtain a separate commercial license.

For commercial licensing terms, custom ports, or consulting, reach out directly via GitHub: [@kniajevich](https://github.com/kniajevich).

---

## License

This ported software and ESP32-P4 modifications are licensed under the terms of the **GNU General Public License v3.0 (GPL-3.0)**.
See the [LICENSE](LICENSE) file for complete details.

Upstream Teensy Audio Library codebase is Copyright (c) PJRC.COM, LLC and licensed under the MIT License.

---

## Support & Sponsorship

If you find this port useful for your audio projects, Eurorack modules, or commercial hardware, consider supporting ongoing DSP optimization and RISC-V hardware verification:

| Network / Asset | Address |
| :--- | :--- |
| **EVM** *(ETH, Arbitrum, Base, BSC, Polygon)* | `0xfd8029b4295d6f4d8AB77Bfc061BBaff4f143C44` |
| **USDT / Tron** *(TRC-20)* | `TQMmeTMudPqafdNqsRnwRGeupBNinLbLdt` |
| **Bitcoin** *(Native SegWit)* | `bc1q9rwm4p5vplqt5a0mdat3vmegm2kgk38zfzlfd7` |
| **Solana** *(SOL, SPL)* | `GKbakfAXmaEXZ4DPTgDJCiZTCtXe9a1qgT5q1Y1hXm35` |
| **Stellar** *(XLM)* | `GACXOMM26U4GL2JT3TEGS2MV5IF7O43Y7X2RGYRUZT2RWQODIOERRITD` |

---
