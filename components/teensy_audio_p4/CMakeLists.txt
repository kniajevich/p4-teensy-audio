# CMake configuration for the teensy_audio_p4 component

idf_component_register(
    SRCS
        src/audio_output_i2s.cpp
        src/audio_synth_waveform_sine.cpp
        src/filter_variable.cpp
        src/mixer.cpp
    INCLUDE_DIRS
        include
    REQUIRES
        driver
)

# Ensure the mock arm_math header is available
target_include_directories(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../arm_math_mock")