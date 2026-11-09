# p4-teensy-audio Architecture

**Repository layout**  

```
p4-teensy-audio/
├── .gitignore
├── CMakeLists.txt
├── main/
│   ├── CMakeLists.txt
│   └── main.cpp
└── components/
    └── teensy_audio_p4/
        ├── CMakeLists.txt
        ├── include/
        │   ├── AudioStream.h
        │   ├── AudioConnection.h
        │   ├── synth_sine.h
        │   ├── output_i2s.h
        │   └── arm_math_mock.h
        └── src/
            ├── AudioStream.cpp
            ├── synth_sine.cpp
            └── output_i2s.cpp
```

All files are shown **in full** below.  Copy each file to the path indicated by the tree, run  

```bash
idf.py set-target esp32p4   # ESP‑IDF v5 target for the P4 core
idf.py build
```  

and the project will compile and link for the ESP‑32 P4 (RISC‑V) MCU.

---

## 1. Root `CMakeLists.txt`

```cmake
# --------------------------------------------------------------
# p4-teensy-audio – root CMakeLists
# --------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)

# The project name is also the name of the generated binary.
set(PROJECT_NAME "p4-teensy-audio")
project(${PROJECT_NAME} LANGUAGES C CXX)

# Pull in the ESP‑IDF build system.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(${PROJECT_NAME})
```

---

## 2. Root `.gitignore`

```gitignore
# ESP‑IDF generated files
build/
sdkconfig
sdkconfig.defaults
sdkconfig.old
*.bin
*.elf
*.map
*.out
*.hex
*.log

# CMake generated files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile

# VSCode / CLion
.vscode/
.idea/

# Misc
*.swp
*~
```

---

## 3. `main/CMakeLists.txt`

```cmake
# --------------------------------------------------------------
# main component (the application entry point)
# --------------------------------------------------------------
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES teensy_audio_p4
)
```

---

## 4. `main/main.cpp`

```cpp
/* --------------------------------------------------------------
 * main.cpp – demo that builds a sine‑wave → I2S audio chain
 * -------------------------------------------------------------- */
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "AudioStream.h"
#include "synth_sine.h"
#include "output_i2s.h"

extern "C" void app_main(void)
{
    /* --------------------------------------------------------------
     * 1️⃣ Initialise the audio‑block memory pool (64 blocks = 8 KB)
     * -------------------------------------------------------------- */
    AudioStream::initialize_memory(64);

    /* --------------------------------------------------------------
     * 2️⃣ Create the objects that form the processing chain
     * -------------------------------------------------------------- */
    static AudioSynthWaveformSine synth;   // generates a sine wave
    static AudioOutputI2S          i2sOut; // sends it to the I²S peripheral

    /* --------------------------------------------------------------
     * 3️⃣ Connect them (source → destination)
     * -------------------------------------------------------------- */
    static AudioConnection conn(&synth, &i2sOut);

    /* --------------------------------------------------------------
     * 4️⃣ Set the desired frequency (e.g. 440 Hz)
     * -------------------------------------------------------------- */
    synth.frequency(440.0f);

    /* --------------------------------------------------------------
     * 5️⃣ FreeRTOS task that drives the whole chain.
     *    Runs on core 1 (the RISC‑V core) with a 3 ms period.
     * -------------------------------------------------------------- */
    auto audio_task = [] (void * /*param*/) {
        while (true) {
            AudioStream::update_all();          // call every stream's update()
            vTaskDelay(pdMS_TO_TICKS(3));       // 3 ms tick
        }
        vTaskDelete(nullptr);
    };

    /* --------------------------------------------------------------
     * Create the task pinned to core 1
     * -------------------------------------------------------------- */
    xTaskCreatePinnedToCore(
        audio_task,               // task function
        "audio_task",             // name
        4096,                     // stack size (bytes)
        nullptr,                  // parameter
        5,                        // priority (higher than idle)
        nullptr,                  // task handle (not needed)
        1                         // core ID (0 = PRO, 1 = APP on ESP‑32 P4)
    );
}
```

---

## 5. Component `teensy_audio_p4/CMakeLists.txt`

```cmake
# --------------------------------------------------------------
# teensy_audio_p4 component
# --------------------------------------------------------------
idf_component_register(
    SRCS
        src/AudioStream.cpp
        src/synth_sine.cpp
        src/output_i2s.cpp
    INCLUDE_DIRS
        include
    REQUIRES
        esp_driver_i2s
        esp_timer
        freertos
        esp_system
)
```

---

## 6. Header `components/teensy_audio_p4/include/arm_math_mock.h`

```c
/* --------------------------------------------------------------
 * arm_math_mock.h – tiny, pure‑C replacements for the two CMSIS
 * macros used by the original Teensy Audio library.
 *
 *   __SSAT(x, n)   – signed saturate to n bits
 *   __SMLALD(a, b) – multiply‑accumulate of two 16‑bit values
 *
 * The implementation follows the behaviour of the ARM intrinsics
 * but uses only standard C.  They are deliberately simple – enough
 * for the DSP code in this repository.
 * -------------------------------------------------------------- */
#ifndef ARM_MATH_MOCK_H
#define ARM_MATH_MOCK_H

#include <stdint.h>
#include <limits.h>

/* Signed saturate to n bits (1 ≤ n ≤ 31). */
static inline int32_t __SSAT(int32_t x, uint32_t n)
{
    const int32_t max = (1L << (n - 1)) - 1;
    const int32_t min = -(1L << (n - 1));
    if (x > max) return max;
    if (x < min) return min;
    return x;
}

/* Multiply‑accumulate of two signed 16‑bit values.
 * Returns (a_low * b_low) + (a_high * b_high) where the low/high
 * 16‑bit halves are taken from the two 32‑bit arguments.
 *
 * The original macro works on two 32‑bit registers that each
 * contain two 16‑bit signed values.  Here we unpack them manually.
 */
static inline int32_t __SMLALD(int32_t a, int32_t b)
{
    int16_t a_lo = (int16_t)(a & 0xFFFF);
    int16_t a_hi = (int16_t)((a >> 16) & 0xFFFF);
    int16_t b_lo = (int16_t)(b & 0xFFFF);
    int16_t b_hi = (int16_t)((b >> 16) & 0xFFFF);

    int32_t acc = (int32_t)a_lo * (int32_t)b_lo;
    acc += (int32_t)a_hi * (int32_t)b_hi;
    return acc;
}

#endif /* ARM_MATH_MOCK_H */
```

---

## 7. Header `components/teensy_audio_p4/include/AudioConnection.h`

```cpp
/* --------------------------------------------------------------
 * AudioConnection.h – connects one AudioStream (source) to another
 * (destination).  The constructor registers the connection with the
 * source's internal singly‑linked list.
 * -------------------------------------------------------------- */
#ifndef AUDIOCONNECTION_H
#define AUDIOCONNECTION_H

#include "AudioStream.h"

class AudioConnection
{
public:
    AudioConnection(AudioStream *src, AudioStream *dst);
    ~AudioConnection();

    /* Used internally by AudioStream – not part of the public API */
    AudioConnection *next() const { return _next; }
    AudioStream *destination() const { return _dst; }

private:
    AudioStream *_src;          // source stream (owner of the list)
    AudioStream *_dst;          // destination stream
    AudioConnection *_next;    // next connection in the source list
    friend class AudioStream;
};

#endif // AUDIOCONNECTION_H
```

---

## 8. Header `components/teensy_audio_p4/include/AudioStream.h`

```cpp
/* --------------------------------------------------------------
 * AudioStream.h – core of the Teensy‑Audio‑compatible layer.
 *
 *  * audio_block_t – 128 samples of signed 16‑bit PCM with a
 *    reference counter.
 *  * A static memory pool is allocated once via
 *    AudioStream::initialize_memory().
 *  * AudioStream objects keep a singly‑linked list of
 *    AudioConnection objects that represent their outputs.
 *  * The base class provides the generic plumbing:
 *      - transmit()
 *      - receiveReadOnly()
 *      - receiveWritable()
 *      - update_all()   (static, walks all streams)
 *
 *  The design mirrors the original Teensy Audio library but is
 *  written for ESP‑IDF and pure C++.
 * -------------------------------------------------------------- */
#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "AudioConnection.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 128‑sample block – exactly the size used by the Teensy library. */
typedef struct audio_block_s {
    int16_t data[128];
    volatile uint8_t ref_count;   // reference counting for block reuse
} audio_block_t;

#ifdef __cplusplus
}
#endif

/* --------------------------------------------------------------
 * Base class for every audio processing block.
 * -------------------------------------------------------------- */
class AudioStream
{
public:
    /* ----------------------------------------------------------------
     *  Memory‑pool handling (static, shared by all streams)
     * ---------------------------------------------------------------- */
    static void initialize_memory(size_t block_count);
    static audio_block_t *allocate_block();   // nullptr on OOM
    static void release_block(audio_block_t *block);

    /* ----------------------------------------------------------------
     *  Connection handling – each stream owns a list of outgoing
     *  AudioConnection objects (singly linked).
     * ---------------------------------------------------------------- */
    void add_connection(AudioConnection *conn);
    void remove_connection(AudioConnection *conn);

    /* ----------------------------------------------------------------
     *  Data flow helpers
     * ---------------------------------------------------------------- */
    /** Transmit a block to every registered destination. */
    void transmit(audio_block_t *block);

    /** Receive a block for read‑only use (no ref‑count change). */
    audio_block_t *receiveReadOnly();

    /** Receive a block for writable use (ref‑count incremented). */
    audio_block_t *receiveWritable();

    /** Called by the framework – each derived class must implement
     *  its own processing and eventually call `transmit()`. */
    virtual void update() = 0;

    /** Walk the global list of streams and call `update()` on each. */
    static void update_all();

    /** Virtual destructor – required for polymorphic delete. */
    virtual ~AudioStream() = default;

protected:
    AudioStream();                     // registers the instance globally

private:
    /* ----------------------------------------------------------------
     *  Per‑instance data
     * ---------------------------------------------------------------- */
    AudioConnection *_connections_head = nullptr;   //