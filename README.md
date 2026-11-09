# p4-teensy-audio 🎛️  

[![Build ESP32-P4 Project](https://github.com/kniajevich/p4-teensy-audio/actions/workflows/build.yml/badge.svg)](https://github.com/kniajevich/p4-teensy-audio/actions/workflows/build.yml)  
![Platform](https://img.shields.io/badge/Platform-ESP32--P4%20(RISC--V)-blue)  
![Framework](https://img.shields.io/badge/Framework-ESP--IDF%20v5.3%2B-red)  
![License](https://img.shields.io/badge/License-MIT-green)  

A high‑performance port and compatibility layer of the legendary **Teensy Audio Library** for the **Espressif ESP32‑P4** dual‑core RISC‑V platform.  
Designed for hardware synth builders, Eurorack module designers, audio engineers, and creative coders who want Teensy Audio’s dynamic node‑graph routing paired with the raw horsepower of ESP32‑P4.

---

## ⚡ Current Features  

- **Core Audio Graph Architecture** – Zero‑copy block streaming (`audio_block_t`, 128 samples @ 44.1 kHz).  
- **SPIRAM / PSRAM Buffering** – Automatic block‑pool allocation in high‑speed PSRAM with internal SRAM fallback.  
- **Dedicated DSP FreeRTOS Core** – Execution pinned to Core 1 for glitch‑free low‑latency processing.  
- **RISC‑V Compatibility Layer** – Integer‑only DSP helpers (`__SSAT`, `__SMLALD`, …) re‑implemented for the RISC‑V toolchain.  
- **Modern ESP‑IDF v5 Driver** – I2S stereo transmission via `esp_driver_i2s` with DMA queues.  
- **Continuous Integration** – GitHub Actions builds on `espressif/idf:release-v5.3`.

### 📦 Implemented Modules  

| Category | Class | Description |
|----------|-------|-------------|
| **Control** | `AudioStream` | Base processing block & dynamic buffer pool |
| **Routing** | `AudioConnection` | Zero‑overhead audio cord patch connector |
| **Output** | `AudioOutputI2S` | 16‑bit stereo I2S master driver (DMA) |
| **Synth** | `AudioSynthWaveformSine` | DDS sine oscillator (32‑bit phase accumulator, 440 Hz example) |
| **Filter** | `AudioFilterStateVariable` | Chamberlin SVF (Low‑pass, Band‑pass, High‑pass) |
| **Mixer** | `AudioMixer4` | 4‑channel mixer with per‑channel gain & saturation guard |

---

## 🔌 Audio Graph Pipeline  

```mermaid
flowchart LR
    A[AudioSynthWaveformSine<br/>(440 Hz)] -->|output| B[AudioFilterStateVariable<br/>Chamberlin SVF<br/>cutoff 1200 Hz]
    B -->|low‑pass| C[AudioMixer4<br/>Gain = 1.0]
    C -->|ch0| D[AudioOutputI2S<br/>Stereo DMA → DAC]
    C -->|ch1| D
```

*ASCII representation*  

```
[AudioSynthWaveformSine] (440 Hz)
        │
        ▼
[AudioFilterStateVariable] (Chamberlin SVF, 1200 Hz)
        │
        ▼
[AudioMixer4] (Gain = 1.0)
   ┌─────┴─────┐
   ▼           ▼
[AudioOutputI2S] (Stereo DMA → DAC)
```

---

## 🚀 Quick Start  

### Prerequisites  

- ESP‑IDF v5.3+ installed and configured (see the [ESP‑IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)).  
- Target hardware: ESP32‑P4 development board (dual‑core RISC‑V).  

### Build & Flash  

```bash
# Clone the repository
git clone https://github.com/kniajevich/p4-teensy-audio.git
cd p4-teensy-audio

# Select the ESP32‑P4 target
idf.py set-target esp32p4

# Build the project
idf.py build

# Flash and monitor (replace /dev/ttyUSB0 with your serial port)
idf.py -p /dev/ttyUSB0 flash monitor
```

The default example creates a 440 Hz sine wave, runs it through a 1.2 kHz low‑pass filter, mixes it (single channel), and outputs stereo audio via I2S.

---

## 🎧 For DIY Musicians & Embedded Hackers  

Tag your projects with:  

```
#ESP32P4 #TeensyAudio #Synthesizer #DSP #RISCV #AudioDev #OpenSource
```

Feel free to fork, extend the node library (add FM synths, reverb, etc.), and share your patches!

---

## ☕ Support & Hardware Donations  

If this port helps your music or hardware projects, consider supporting further development:  

- **TON** (TRC‑20): `YOUR_TON_WALLET_ADDRESS_HERE`  
- **USDT (TRC‑20)**: `YOUR_USDT_WALLET_ADDRESS_HERE`  

Your contributions keep the project alive and enable new modules.

---

## 📄 License  

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.  

---  

*Happy hacking and keep the sound alive!*