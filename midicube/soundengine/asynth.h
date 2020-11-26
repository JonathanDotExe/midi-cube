/*
 * presetsynth.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_
#define MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_

#include "soundengine.h"
#include "../oscilator.h"
#include "../filter.h"
#include "../util.h"

#define ANALOG_OSCILATOR_COUNT 8
#define ANALOG_MOD_ENV_COUNT 8
#define ANALOG_LFO_COUNT 8
#define ANALOG_CONTROL_COUNT 128

const FixedScale VOLUME_SCALE = {0, {}, 1};
const FixedScale SYNC_SCALE = {0, {}, 1};
const FixedScale PULSE_WIDTH_SCALE = {0, {}, 0.5};
const FixedScale UNISON_DETUNE_SCALE = {0, {}, 1};
const FixedScale FILTER_CUTOFF_SCALE(0, {}, 1);
const FixedScale FILTER_RESONANCE_SCALE(0, {}, 1);
const FixedScale PITCH_SCALE(-2, {}, 2);
const FixedScale PANNING_SCALE(-1, {}, 1);

struct PropertyModulation {
	double value = 0;
	size_t mod_env = 0;
	double mod_amount = 0;
	size_t lfo = 0;
	double lfo_amount = 0;
	double velocity_amount = 0;
	size_t cc = 1;
	double cc_amount = 0;
};

struct OscilatorEntity {
	bool active = false;
	ADSREnvelope env{0.0, 0, 1, 0.0};
	AnalogWaveForm waveform = AnalogWaveForm::SAW_DOWN;
	bool analog = true;
	bool sync = false;
	size_t unison_amount = 0;
	PropertyModulation volume = {1};
	PropertyModulation sync_mul = {0.1};
	PropertyModulation pulse_width = {1};
	PropertyModulation unison_detune = {0.1};
	PropertyModulation panning = {0.5};
	int semi = 0;
	double transpose = 2;
	PropertyModulation pitch = {0.5};

	bool filter = false;
	FilterType filter_type = FilterType::LP_12;
	PropertyModulation filter_cutoff = {1};
	PropertyModulation filter_resonance = {0};
};

struct ModEnvelopeEntity {
	bool active = false;
	PropertyModulation volume = {1};
	ADSREnvelope env{0.0, 0, 1, 0.0};
};

struct LFOEntity {
	bool active = false;
	PropertyModulation volume = {1};
	double freq = 1;
	AnalogWaveForm waveform = AnalogWaveForm::SINE;
};

struct AnalogSynthPreset {
	std::array<LFOEntity, ANALOG_LFO_COUNT> lfos;
	std::array<ModEnvelopeEntity, ANALOG_MOD_ENV_COUNT> mod_envs;
	std::array<OscilatorEntity, ANALOG_OSCILATOR_COUNT> oscilators;
};

class AnalogSynth : public BaseSoundEngine {

private:
	AnalogOscilatorBank<SOUND_ENGINE_POLYPHONY * ANALOG_OSCILATOR_COUNT, 8> oscilators;
	std::array<Filter, SOUND_ENGINE_POLYPHONY * ANALOG_OSCILATOR_COUNT> filters;
	std::array<AnalogOscilator, ANALOG_LFO_COUNT> lfos;
	std::array<double, ANALOG_CONTROL_COUNT> controls;

public:
	AnalogSynthPreset preset;

	AnalogSynth();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env);

	~AnalogSynth();

};


#endif /* MIDICUBE_SOUNDENGINE_PRESETSYNTH_H_ */
