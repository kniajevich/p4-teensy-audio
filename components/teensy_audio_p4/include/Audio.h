#pragma once

// Core Engine
#include "AudioStream.h"
#include "output_i2s.h"
#include "input_i2s.h"

// Generators
#include "synth_sine.h"
#include "synth_whitenoise.h"
#include "synth_pinknoise.h"
#include "synth_waveform.h"
#include "synth_waveform_mod.h"

// Filters
#include "filter_variable.h"
#include "filter_biquad.h"

// Routing & Level
#include "mixer.h"
#include "mixer8.h"
#include "control_amp.h"
#include "control_pan.h"
#include "effect_passthrough.h"

// Effects
#include "effect_envelope.h"
#include "effect_delay.h"
#include "effect_bitcrusher.h"
#include "effect_multiply.h"
#include "effect_waveshaper.h"

// Analysis
#include "analyze_peak.h"
#include "analyze_rms.h"
#include "analyze_fft1024.h"
