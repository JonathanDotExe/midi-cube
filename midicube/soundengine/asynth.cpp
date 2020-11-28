/*
 * asynth.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: jojo
 */
#include "asynth.h"
#include <cmath>


AnalogSynth::AnalogSynth() {
	//Squashy Synth Basss
	/*ModEnvelopeEntity& mod_env = preset.mod_envs.at(0);
	mod_env.active = true;
	mod_env.env = {0, 0.1, 0, 0.003};

	OscilatorEntity& osc = preset.oscilators.at(0);
	osc.active = true;
	osc.env = {0.0005, 0.35, 0, 0.06};
	osc.filter = true;
	osc.filter_type = FilterType::LP_24;
	osc.filter_cutoff.value = 0.05;
	osc.filter_cutoff.mod_env = 0;
	osc.filter_cutoff.mod_amount = 0.15;
	osc.filter_resonance.value = 0.8;*/

	//Unison Saw
	/*OscilatorEntity& osc = preset.oscilators.at(0);
	osc.reset = true;
	osc.unison_amount = 2;
	osc.active = true;
	osc.env = {0.0005, 0, 1, 0.003};*/

	//Poly Sweep
	/*OscilatorEntity& osc1 = preset.oscilators.at(0);
	osc1.unison_amount = 3;
	osc1.unison_detune.value = 0.05;
	osc1.active = true;
	osc1.env = {0.0005, 0, 1, 0.003};
	osc1.filter = true;
	osc1.filter_type = FilterType::LP_24;
	osc1.filter_cutoff.value = 0.0;
	osc1.filter_cutoff.cc_amount = 1;

	OscilatorEntity& osc2 = preset.oscilators.at(1);
	osc2.unison_amount = 3;
	osc2.unison_detune.value = 0.05;
	osc2.semi = 12;
	osc2.active = true;
	osc2.env = {0.0005, 0, 1, 0.003};
	osc2.filter = true;
	osc2.filter_type = FilterType::LP_24;
	osc2.filter_cutoff.value = 0.0;
	osc2.filter_cutoff.cc_amount = 1;

	OscilatorEntity& osc3 = preset.oscilators.at(2);
	osc3.waveform = AnalogWaveForm::SINE;
	osc3.unison_amount = 7;
	osc3.unison_detune.value = 0.05;
	osc3.semi = 12;
	osc3.active = true;
	osc3.env = {0.0005, 0, 1, 0.003};
	osc3.filter = true;
	osc3.filter_type = FilterType::LP_24;
	osc3.filter_cutoff.value = 0.0;
	osc3.filter_cutoff.cc_amount = 1;*/

	//Spooky Sine
	preset.mono = true;
	preset.portamendo = 0.1;

	LFOEntity& lfo = preset.lfos.at(0);
	lfo.active = true;
	lfo.freq = 6;
	lfo.volume.value = 0;
	lfo.volume.cc_amount = 1;

	OscilatorEntity& osc = preset.oscilators.at(0);
	osc.waveform = AnalogWaveForm::SINE;
	osc.active = true;
	osc.env = {0.005, 0, 1, 0.005};
	osc.pitch.lfo = 0;
	osc.pitch.lfo_amount = 0.125;

	//Lush Lead
	/*LFOEntity& lfo = preset.lfos.at(0);
	lfo.active = true;
	lfo.freq = 0.5;

	OscilatorEntity& osc = preset.oscilators.at(0);
	osc.waveform = AnalogWaveForm::SAW_DOWN;
	osc.active = true;
	osc.env = {0.0005, 0.35, 0.7, 0.003};
	osc.filter = true;
	osc.filter_type = FilterType::LP_24;
	osc.filter_cutoff.value = 0.2;
	osc.filter_resonance.value = 0.0;
	osc.filter_kb_track = 1;
	osc.panning.value = 0;
	osc.panning.lfo_amount = 1;*/
}

static double apply_modulation(const FixedScale& scale, PropertyModulation& mod, std::array<double, ANALOG_MOD_ENV_COUNT>& env_val, std::array<double, ANALOG_LFO_COUNT>& lfo_val, std::array<double, ANALOG_CONTROL_COUNT>& controls, double velocity) {
	double prog = mod.value;
	prog += env_val.at(mod.mod_env) * mod.mod_amount + lfo_val.at(mod.lfo) * mod.lfo_amount + controls.at(mod.cc) * mod.cc_amount + velocity * mod.velocity_amount;
	prog = fmin(fmax(prog, 0.0), 1.0);
	return scale.value(prog);
}

void AnalogSynth::process_note(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	env_val = {};
	//Mod Envs
	for (size_t i = 0; i < preset.mod_envs.size(); ++i) {
		ModEnvelopeEntity& mod_env = preset.mod_envs[i];
		if (mod_env.active) {
			double volume = apply_modulation(VOLUME_SCALE, mod_env.volume, env_val, lfo_val, controls, note.velocity);
			env_val.at(i) = mod_env.env.amplitude(info.time, note, env) * volume;
		}
	}
	//Synthesize
	double lsample = 0;
	double rsample = 0;
	for (size_t i = 0; i < preset.oscilators.size(); ++i) {
		OscilatorEntity& osc = preset.oscilators[i];
		if (osc.active) {
			//Frequency
			double freq = note.freq;
			double pitch = apply_modulation(PITCH_SCALE, osc.pitch, env_val, lfo_mod, controls, note.velocity);
			if (osc.semi || pitch) {
				freq = note_to_freq(note.note + osc.semi + pitch);
			}
			freq *= env.pitch_bend * osc.transpose;

			AnalogOscilatorData data = {osc.waveform, osc.analog, osc.sync};
			AnalogOscilatorBankData bdata = {0.1, osc.unison_amount};
			//Apply modulation
			double volume = apply_modulation(VOLUME_SCALE, osc.volume, env_val, lfo_val, controls, note.velocity) * osc.env.amplitude(info.time, note, env);
			data.sync_mul = apply_modulation(SYNC_SCALE, osc.sync_mul, env_val, lfo_val, controls, note.velocity);
			data.pulse_width = apply_modulation(PULSE_WIDTH_SCALE, osc.pulse_width, env_val, lfo_val, controls, note.velocity);
			bdata.unison_detune = apply_modulation(UNISON_DETUNE_SCALE, osc.unison_detune, env_val, lfo_val, controls, note.velocity);

			//Signal
			size_t index = note_index + i * SOUND_ENGINE_POLYPHONY;
			//Only on note start
			if (note.start_time + info.time_step > info.time) {
				if (osc.reset) {
					oscilators.reset(index);
				}
				else if (osc.randomize) {
					oscilators.randomize(index);
				}
			}

			double signal = oscilators.signal(freq, info.time_step, index, data, bdata).carrier;
			//Filter
			if (osc.filter) {
				FilterData filter{osc.filter_type};
				filter.cutoff = apply_modulation(FILTER_CUTOFF_SCALE, osc.filter_cutoff, env_val, lfo_val, controls, note.velocity);
				filter.resonance = apply_modulation(FILTER_RESONANCE_SCALE, osc.filter_resonance, env_val, lfo_val, controls, note.velocity);

				if (osc.filter_kb_track && filter.cutoff != 1) {
					double cutoff = factor_to_cutoff(filter.cutoff, info.time_step);
					cutoff *= 1 + ((double) note.note - 36)/12.0 * osc.filter_kb_track;
					filter.cutoff = cutoff_to_factor(cutoff, info.time_step);
				}

				signal = filters.at(note_index + i * SOUND_ENGINE_POLYPHONY).apply(filter, signal, info.time_step);
			}
			signal *= volume;
			//Pan
			double panning = apply_modulation(PANNING_SCALE, osc.panning, env_val, lfo_val, controls, note.velocity);
			lsample += signal * (2 - panning);
			rsample += signal * (1 + panning);
		}
	}

	//Play
	for (size_t i = 0; i < channels.size(); ++i) {
		if (i%2 == 0) {
			channels[i] += lsample;
		}
		else {
			channels[i] += rsample;
		}
	}
}

void AnalogSynth::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	if (!preset.mono) {
		process_note(channels, info, note, env, note_index);
	}
}

void AnalogSynth::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status) {
	//Mono
	if (preset.mono && status.latest_note) {
		//Update portamendo
		note_port.set(status.latest_note->note, info.time, first_port ? 0 : preset.portamendo);
		first_port = false;
		double pitch = note_port.get(info.time);
		KeyboardEnvironment e = env;
		e.pitch_bend *= note_to_freq_transpose(pitch - status.latest_note->note);

		process_note(channels, info, *status.latest_note, e, 0);
	}
	//Move LFOs
	//TODO move before notes
	lfo_val = {};
	lfo_mod = {};
	for (size_t i = 0; i < preset.lfos.size(); ++i) {
		LFOEntity& lfo = preset.lfos[i];
		if (lfo.active) {
			double volume = apply_modulation(VOLUME_SCALE, lfo.volume, env_val, lfo_val, controls, 0);
			AnalogOscilatorData d = {lfo.waveform};
			AnalogOscilatorSignal sig = lfos.at(i).signal(lfo.freq, info.time_step, d);
			lfo_val.at(i) = sig.carrier * volume;
			lfo_mod.at(i) = sig.modulator * volume;
		}
	}
}

void AnalogSynth::control_change(unsigned int control, unsigned int value) {
	controls.at(control) = value/127.0;
}

bool AnalogSynth::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	bool finished = true;
	for (size_t i = 0; i < preset.oscilators.size() && finished; ++i) {
		OscilatorEntity& osc = preset.oscilators[i];
		if (osc.active) {
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

