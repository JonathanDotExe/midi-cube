/*
 * asynth.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: jojo
 */
#include "asynth.h"


AnalogSynth::AnalogSynth() {
	OscilatorEntity& osc = preset.oscilators.at(0);
	osc.volume = 1;
	osc.env = {0.0005, 0, 1, 0.003};
	osc.unison_amount = 2;
}

static double apply_modulation(const FixedScale& scale, PropertyModulation& mod, std::array<double, ANALOG_MOD_ENV_COUNT>& env_val, std::array<double, ANALOG_LFO_COUNT>& lfo_val, double velocity) {
	double prog = mod.value;
	prog += env_val.at(mod.mod_env) * mod.mod_amount + lfo_val.at(mod.lfo) * mod.lfo_amount + velocity * mod.velocity_amount;
	prog = std::min(std::max(prog, 0.0), 1.0);
	return scale.value(prog);
}

void AnalogSynth::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	//Mod Envs
	std::array<double, ANALOG_MOD_ENV_COUNT> env_val;
	for (size_t i = 0; i < preset.mod_envs.size(); ++i) {
		ModEnvelopeEntity& mod_env = preset.mod_envs[i];
		if (mod_env.volume) {
			env_val.at(i) = mod_env.env.amplitude(info.time, note, env) * mod_env.volume;
		}
		else {
			env_val.at(i) = 0;
		}
	}
	//LFOs
	std::array<double, ANALOG_LFO_COUNT> lfo_val;
	for (size_t i = 0; i < preset.lfos.size(); ++i) {
		LFOEntity& lfo = preset.lfos[i];
		if (lfo.volume) {
			AnalogOscilatorData d = {lfo.waveform};
			lfo_val.at(i) = lfos.at(i).signal(lfo.freq, info.time_step, d, false) * lfo.volume;
		}
		else {
			lfo_val.at(i) = 0;
		}
	}
	//Synthesize
	double sample = 0;
	for (size_t i = 0; i < preset.oscilators.size(); ++i) {
		OscilatorEntity& osc = preset.oscilators[i];
		if (osc.volume) {
			AnalogOscilatorData data = {osc.waveform, osc.analog, osc.sync};
			AnalogOscilatorBankData bdata = {0.1, osc.unison_amount};
			//Apply modulation
			data.sync_mul = apply_modulation(SYNC_SCALE, osc.sync_mul, env_val, lfo_val, note.velocity);
			data.pulse_width = apply_modulation(PULSE_WIDTH_SCALE, osc.pulse_width, env_val, lfo_val, note.velocity);
			bdata.unison_detune = apply_modulation(UNISON_DETUNE_SCALE, osc.unison_detune, env_val, lfo_val, note.velocity);

			sample += oscilators.signal(note.freq, info.time_step, note_index + i * SOUND_ENGINE_POLYPHONY, data, bdata, false) * osc.volume;
		}
	}

	//Play
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += sample;
	}
}

void AnalogSynth::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status) {

}

void AnalogSynth::control_change(unsigned int control, unsigned int value) {

}

bool AnalogSynth::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	bool finished = true;
	for (size_t i = 0; i < preset.oscilators.size() && finished; ++i) {
		OscilatorEntity& osc = preset.oscilators[i];
		if (osc.volume) {
			finished = osc.env.is_finished(info.time, note, env);
		}
	}
	return finished;
}

AnalogSynth::~AnalogSynth() {

}

template<>
std::string get_engine_name<AnalogSynth>() {
	return "Analog Synthesizer";
}

