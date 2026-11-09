# p4-teensy-audio 🎛️  

[![Build ESP32-P4 Project](https://github.com/kniajevich/p4-teensy-audio/actions/workflows/build.yml/badge.svg)](https://github.com/kniajevich/p4-teensy-audio/actions/workflows/build.yml)  
![Platform](https://img.shields.io/badge/Platform-ESP32--P4%20(RISC--V)-blue)  
![Framework](https://img.shields.io/badge/Framework-ESP--IDF%20v5.3+-red)  
![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)  

High-performance Teensy Audio compatibility engine ported to the **Espressif ESP32-P4** dual-core RISC-V microcontroller.

---

## 🙏 Acknowledgements & Attribution  

This project is built upon the foundational concepts and DSP architecture of the **Teensy Audio Library**, designed by **Paul Stoffregen** ([PJRC.COM](https://www.pjrc.com/)).  

The original Teensy Audio Library is distributed under the permissive **MIT License**. This port bridges Paul's node-based streaming paradigm to modern RISC-V embedded hardware and ESP-IDF v5 DMA pipelines.

---

## ⚡ Key Features  

- **Zero-Copy Block Streaming**: Native 128 samples @ 44.1 kHz `audio_block_t` allocation.  
- **Dedicated DSP Task**: Pinned to FreeRTOS Core 1 for stable jitter-free execution.  
- **RISC-V Math Emulation**: Pure integer saturation and multiply-accumulate (`__SSAT`, `__SMLALD`).  
- **High-Speed PSRAM Support**: Block pool allocation optimized for polyphony.  
- **ESP-IDF v5 Standard I2S**: Stereo master transmission with DMA queues.

### 📦 Implemented Modules  

| Category | Class | Description |
| :--- | :--- | :--- |
| **Core** | `AudioStream` | Base processing block & dynamic buffer manager |
| **Routing** | `AudioConnection` | High-speed patch cord connector |
| **Output** | `AudioOutputI2S` | 16-bit stereo master driver (DMA) |
| **Synth** | `AudioSynthWaveformSine` | DDS sine oscillator (32-bit phase accumulator) |
| **Filter** | `AudioFilterStateVariable` | Chamberlin SVF (Lowpass, Bandpass, Highpass) |
| **Mixer** | `AudioMixer4` | 4-channel saturation-guarded mixer |

---

## 🔌 Signal Flow Diagram  

[AudioSynthWaveformSine] ──> [AudioFilterStateVariable] ──(LP)──> [AudioMixer4] ──> [AudioOutputI2S]

---

## 🚀 Quick Start  

```bash
git clone https://github.com/kniajevich/p4-teensy-audio.git
cd p4-teensy-audio
idf.py set-target esp32p4
idf.py build
```

⚖️ Licensing & Commercial Inquiries  

Open Source / DIY Community: Free to use, fork, and build under the terms of the GNU General Public License v3.0 (GPLv3).  

Commercial & Closed-Source Use: If you are manufacturing hardware products or closed-source firmware and cannot release your project under GPLv3, please contact the author (kniajevich) for commercial dual-licensing terms.

🏷️ Tags for Audio Builders  

#ESP32P4 #TeensyAudio #Synthesizer #Eurorack #DSP #RISCV #EmbeddedCPP #OpenSource