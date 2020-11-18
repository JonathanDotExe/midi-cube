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
#include "../util.h"

#define ANALOG_OSCILATOR_COUNT 8
#define ANALOG_MOD_ENV_COUNT 8
#define ANALOG_LFO_COUNT 8

struct PropertyModulation {
	size_t mod_env = 0;
	double mod_amount = 0;
	size_t lfo = 0;
	double lfo_amount = 0;
	double velocity_amount = 0;
};

struct OscilatorEntity {
	double volume = 0;
	ADSREnvelope env{0.0, 0, 1, 0.0};
	AnalogOscilatorData osc{AnalogWaveForm::SAW_DOWN, true};
	AnalogOscilatorBankData bank;
	PropertyModulation sync_mod;
	PropertyModulation pulse_width_mod;
	PropertyModulation unison_detune_mod;
};

struct ModEnvelopeEntity {
	double volume = 0;
	ADSREnvelope env{0.0, 0, 1, 0.0};
};

struct LFOEntity {
	double volume = 0;
	double freq = 1;
	AnalogWaveForm waveform = AnalogWaveForm::SAW_DOWN;
};

struct AnalogSynthPreset {
	std::array<OscilatorEntity, ANALOG_LFO_COUNT> lfos;
	std::array<OscilatorEntity, ANALOG_MOD_ENV_COUNT> mod_envs;
	std::array<OscilatorEntity, ANALOG_OSCILATOR_COUNT> oscilators;
};

const FixedScale SYNC_SCALE = {0, {}, 1};
const FixedScale PULSE_WIDTH_SCALE = {0, {}, 0.5};
const FixedScale UNISON_DETUNE_SCALE = {0, {}, 1};

class AnalogSynth : public BaseSoundEngine {

private:
	AnalogOscilatorBank<SOUND_ENGINE_POLYPHONY * ANALOG_OSCILATOR_COUNT, 8> oscilators;
	std::array<AnalogOscilator, ANALOG_LFO_COUNT> lfos;

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
