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

#define ANALOG_OSCILATOR_COUNT 8
#define ANALOG_MOD_ENV_COUNT 8
#define ANALOG_LFO_COUNT 8

struct OscilatorEntity {
	double volume = 0;
	ADSREnvelope env{0.0, 0, 1, 0.0};
	AnalogOscilatorData osc;
};

struct ModEnvelopeEntity {
	double volume = 0;
	ADSREnvelope env{0.0, 0, 1, 0.0};
};

struct LFOEnvelopeEntity {
	double volume = 0;
	double freq = 1;
	AnalogWaveForm waveform = AnalogWaveForm::SAW_DOWN;
};

struct AnalogSynthPreset {
	std::array<OscilatorEntity, ANALOG_LFO_COUNT> lfos;
	std::array<OscilatorEntity, ANALOG_MOD_ENV_COUNT> mod_envs;
	std::array<OscilatorEntity, ANALOG_OSCILATOR_COUNT> oscilators;
};

class AnalogSynth : public BaseSoundEngine {

private:


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
