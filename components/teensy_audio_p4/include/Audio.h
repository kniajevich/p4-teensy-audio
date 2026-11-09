#pragma once

// Core Engine
#include "AudioStream.h"
#include "output_i2s.h"
#include "input_i2s.h"
#include "play_queue.h"
#include "record_queue.h"

// Generators & Synthesis
#include "synth_sine.h"
#include "synth_whitenoise.h"
#include "synth_pinknoise.h"
#include "synth_waveform.h"
#include "synth_waveform_mod.h"
#include "synth_karplusstrong.h"
#include "synth_tonesweep.h"

// Filters
#include "filter_variable.h"
#include "filter_biquad.h"
#include "filter_fir.h"

// Routing & Level
#include "mixer.h"
#include "mixer4.h"
#include "mixer8.h"
#include "control_amp.h"
#include "control_pan.h"
#include "effect_passthrough.h"

// Dynamics & Effects
#include "effect_envelope.h"
#include "effect_delay.h"
#include "effect_bitcrusher.h"
#include "effect_multiply.h"
#include "effect_waveshaper.h"
#include "effect_chorus.h"
#include "effect_flange.h"
#include "effect_freeverb.h"

// Analysis
#include "analyze_peak.h"
#include "analyze_rms.h"
#include "analyze_fft1024.h"
#include "analyze_tonedetect.h"
