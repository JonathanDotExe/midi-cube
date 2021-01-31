/*
 * asynth.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: jojo
 */
#include "asynth.h"
#include <cmath>
#include <iostream>

#define OSC_INDEX(note_index,i) (note_index + i * SOUND_ENGINE_POLYPHONY)
#define ENV_INDEX(note_index,i) (note_index + i * SOUND_ENGINE_POLYPHONY)

void apply_preset(SynthFactoryPreset type, AnalogSynthPreset &preset) {
	switch (type) {
	case SQUASHY_BASS: {
		ModEnvelopeEntity &mod_env = preset.mod_envs.at(0);
		mod_env.env = { 0, 0.1, 0, 0.003 };

		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.env = { 0.0005, 0.35, 0, 0.06 };
		osc.filter = true;
		osc.filter_type = FilterType::LP_24;
		osc.filter_cutoff.value = 0.05;
		osc.filter_cutoff.mod_env = 0;
		osc.filter_cutoff.mod_env_amount = 0.15;
		osc.filter_resonance.value = 0.8;

		preset.lfo_count = 0;
		preset.mod_env_count = 1;
		preset.osc_count = 1;
	}
		break;
	case UNISON_SAWS: {
		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.unison_amount = 2;
		osc.env = { 0.0005, 0, 1, 0.003 };

		preset.lfo_count = 0;
		preset.mod_env_count = 0;
		preset.osc_count = 1;
	}
		break;
	case POLY_SWEEP: {
		OscilatorEntity &osc1 = preset.oscilators.at(0);
		osc1.volume.value = 0.4;
		osc1.unison_amount = 7;
		osc1.unison_detune.value = 0.05;
		osc1.env = { 0.0005, 0, 1, 0.1 };
		osc1.filter = true;
		osc1.filter_type = FilterType::LP_24;
		osc1.filter_cutoff.value = 0.1;
		osc1.filter_cutoff.cc_amount = 0.9;

		OscilatorEntity &osc2 = preset.oscilators.at(1);
		osc2.volume.value = 0.4;
		osc2.unison_amount = 3;
		osc2.unison_detune.value = 0.05;
		osc2.semi = 12;
		osc2.env = { 0.0005, 0, 1, 0.1 };
		osc2.filter = true;
		osc2.filter_type = FilterType::LP_24;
		osc2.filter_cutoff.value = 0.1;
		osc2.filter_cutoff.cc_amount = 0.9;

		preset.lfo_count = 0;
		preset.mod_env_count = 0;
		preset.osc_count = 2;
	}
		break;
	case SPOOKY_SINE: {
		preset.mono = true;
		preset.legato = true;
		preset.portamendo = 0.1;

		LFOEntity &lfo = preset.lfos.at(0);
		lfo.freq = 6;
		lfo.volume.value = 0;
		lfo.volume.cc_amount = 1;

		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.waveform = AnalogWaveForm::SINE_WAVE;
		osc.env = { 0.0005, 0, 1, 0.003 };
		osc.pitch.lfo = 0;
		osc.pitch.lfo_amount = 0.125;

		preset.lfo_count = 1;
		preset.mod_env_count = 0;
		preset.osc_count = 1;
	}
		break;
	case LUSH_LEAD: {
		LFOEntity &lfo = preset.lfos.at(0);
		lfo.freq = 0.5;

		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.waveform = AnalogWaveForm::SAW_DOWN_WAVE;
		osc.env = { 0.0005, 0.35, 0.7, 0.003 };
		osc.filter = true;
		osc.filter_type = FilterType::LP_24;
		osc.filter_cutoff.value = 0.2;
		osc.filter_resonance.value = 0.0;
		osc.filter_kb_track = 1;
		osc.panning.value = 0;
		osc.panning.lfo_amount = 1;

		preset.lfo_count = 1;
		preset.mod_env_count = 0;
		preset.osc_count = 1;
	}
		break;
	case PULSE_BASS: {
		ModEnvelopeEntity &mod_env = preset.mod_envs.at(0);
		mod_env.env = { 0, 0.1, 0, 0.1 };

		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.waveform = AnalogWaveForm::SQUARE_WAVE;
		osc.pulse_width.value = 0.66;
		osc.env = { 0.0005, 0.4, 0, 0.003 };
		osc.filter = true;
		osc.filter_type = FilterType::LP_24;
		osc.filter_cutoff.value = 0.05;
		osc.filter_cutoff.mod_env = 0;
		osc.filter_cutoff.mod_env_amount = 0.2;
		osc.filter_resonance.value = 0.5;

		preset.lfo_count = 1;
		preset.mod_env_count = 0;
		preset.osc_count = 1;
	}
		break;
	case DELAY_CHORDS: {
		preset.delay_mix = 0.5;
		preset.delay_feedback = 0.3;
		preset.delay_time = 0.3;

		ModEnvelopeEntity &env = preset.mod_envs.at(0);
		env.env = { 0, 0.2, 0, 0.05 };

		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.unison_amount = 1;
		osc.unison_detune.value = 0.06;
		osc.env = { 0.01, 0, 1, 0.1 };

		osc.filter = true;
		osc.filter_type = FilterType::LP_24;
		osc.filter_kb_track = 1;
		osc.filter_cutoff.value = 0.002;
		osc.filter_cutoff.mod_env_amount = 0.5;
		osc.filter_resonance.value = 0.4;

		preset.lfo_count = 0;
		preset.mod_env_count = 1;
		preset.osc_count = 1;
	}
		break;
	case STRONG_PAD: {
		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.waveform = AnalogWaveForm::SAW_DOWN_WAVE;
		osc.unison_amount = 3;
		osc.unison_detune.value = 0.05;
		osc.randomize = true;
		osc.env = { 0.2, 0, 1, 0.4 };

		osc.filter = true;
		osc.filter_cutoff.value = 0.05;

		preset.lfo_count = 0;
		preset.mod_env_count = 0;
		preset.osc_count = 1;
	}
		break;
	case CLEAN_SAW: {
		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.waveform = AnalogWaveForm::SAW_DOWN_WAVE;
		osc.env = { 0.0005, 0, 1, 0.003 };

		preset.lfo_count = 0;
		preset.mod_env_count = 0;
		preset.osc_count = 1;
	}
		break;
	case FM_BASS: {
		OscilatorEntity &osc1 = preset.oscilators.at(0);
		osc1.transpose = 0.7;
		osc1.waveform = AnalogWaveForm::SINE_WAVE;
		osc1.audible = false;
		osc1.env = { 0, 0.2, 0, 0.003 };

		OscilatorEntity &osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::SINE_WAVE;
		osc2.fm.push_back( { 10, 0 });
		osc2.env = { 0.0005, 0.5, 0.0, 0.003 };

		preset.lfo_count = 0;
		preset.mod_env_count = 0;
		preset.osc_count = 2;
	}
		break;
	case FM_E_PIANO: {
		OscilatorEntity &osc1 = preset.oscilators.at(0);
		osc1.waveform = AnalogWaveForm::SINE_WAVE;
		osc1.audible = false;
		osc1.env = { 0.0005, 0.6, 0.6, 0.2 };

		OscilatorEntity &osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::SINE_WAVE;
		osc2.volume.value = 0.2;
		osc2.volume.velocity_amount = 0.3;
		osc2.fm.push_back( { 0.8, 0 });
		osc2.env = { 0.0005, 1.8, 0.0, 0.1 };

		OscilatorEntity &osc3 = preset.oscilators.at(2);
		osc3.waveform = AnalogWaveForm::SINE_WAVE;
		osc3.volume.value = 0.0;
		osc3.volume.velocity_amount = 1;
		osc3.transpose = 10;
		osc3.audible = false;
		osc3.env = { 0.0005, 0.2, 0.4, 0.2 };

		OscilatorEntity &osc4 = preset.oscilators.at(3);
		osc4.waveform = AnalogWaveForm::SINE_WAVE;
		osc4.volume.value = 0.5;
		osc4.fm.push_back( { 1.5, 2 });
		osc4.env = { 0.0005, 1.7, 0.0, 0.1 };

		preset.lfo_count = 0;
		preset.mod_env_count = 0;
		preset.osc_count = 4;
	}
		break;
	case BRASS_PAD: {
		ModEnvelopeEntity &mod_env = preset.mod_envs.at(0);
		mod_env.env = { 0.06, 0.5, 0, 0.05 };

		ModEnvelopeEntity &mod_env1 = preset.mod_envs.at(1);
		mod_env1.env = { 0.06, 0.7, 0, 0.05 };

		ModEnvelopeEntity &mod_env2 = preset.mod_envs.at(2);
		mod_env2.env = { 0.06, 0.4, 0, 0.05 };

		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.volume.value = 0.3;
		osc.env = { 0.0005, 0, 1, 0.1 };
		osc.filter = true;
		osc.filter_type = FilterType::LP_12;
		osc.filter_cutoff.value = 0.1;
		osc.filter_cutoff.mod_env = 0;
		osc.filter_cutoff.mod_env_amount = 0.3;

		OscilatorEntity &osc1 = preset.oscilators.at(1);
		osc1.volume.value = 0.3;
		osc1.semi = 0.05;
		osc1.panning.value = 0;
		osc1.env = { 0.0005, 0, 1, 0.1 };
		osc1.filter = true;
		osc1.filter_type = FilterType::LP_12;
		osc1.filter_cutoff.value = 0.07;
		osc1.filter_cutoff.mod_env = 1;
		osc1.filter_cutoff.mod_env_amount = 0.3;

		OscilatorEntity &osc2 = preset.oscilators.at(2);
		osc2.volume.value = 0.3;
		osc2.semi = -0.05;
		osc2.panning.value = 1;
		osc2.env = { 0.0005, 0, 1, 0.1 };
		osc2.filter = true;
		osc2.filter_type = FilterType::LP_12;
		osc2.filter_cutoff.value = 0.12;
		osc2.filter_cutoff.mod_env = 2;
		osc2.filter_cutoff.mod_env_amount = 0.3;

		preset.lfo_count = 0;
		preset.mod_env_count = 3;
		preset.osc_count = 3;
	}
		break;
	case FM_KALIMBA: {
		OscilatorEntity &osc1 = preset.oscilators.at(0);
		osc1.waveform = AnalogWaveForm::SINE_WAVE;
		osc1.semi = 28;
		osc1.audible = false;
		osc1.env = { 0.0005, 0.12, 0.0, 0.03 };

		OscilatorEntity &osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::SINE_WAVE;
		osc2.fm.push_back( { 0.4, 0 });
		osc2.env = { 0.0005, 0.8, 0.0, 0.1 };

		preset.delay_mix = 0.25;
		preset.delay_feedback = 0.3;
		preset.delay_time = 0.4;

		preset.lfo_count = 0;
		preset.mod_env_count = 0;
		preset.osc_count = 2;
	}
		break;
	case SYNTH_BRASS: {
		ModEnvelopeEntity &mod_env = preset.mod_envs.at(0);
		mod_env.env = { 0.1, 0.8, 0, 0.05 };

		OscilatorEntity &osc = preset.oscilators.at(0);
		osc.unison_amount = 3;
		osc.unison_detune.value = 0.05;
		osc.env = { 0.0005, 0, 1, 0.003 };
		osc.filter = true;
		osc.filter_type = FilterType::LP_24;
		osc.filter_cutoff.value = 0.2;
		osc.filter_cutoff.mod_env = 0;
		osc.filter_cutoff.mod_env_amount = 0.15;

		preset.lfo_count = 0;
		preset.mod_env_count = 1;
		preset.osc_count = 1;
	}
		break;
	case BELL_LEAD: {
		OscilatorEntity &osc1 = preset.oscilators.at(0);
		osc1.waveform = AnalogWaveForm::TRIANGLE_WAVE;
		osc1.volume.value = 0.35;
		osc1.unison_amount = 2;
		osc1.unison_detune.value = 0.05;
		osc1.env = { 0.0005, 0, 1, 0.07 };

		OscilatorEntity &osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::TRIANGLE_WAVE;
		osc2.volume.value = 0.35;
		osc2.unison_amount = 2;
		osc2.unison_detune.value = 0.05;
		osc2.semi = 24;
		osc2.env = { 0.0005, 0, 1, 0.07 };

		OscilatorEntity &osc3 = preset.oscilators.at(3);
		osc3.waveform = AnalogWaveForm::SAW_DOWN_WAVE;
		osc3.volume.value = 0;
		osc3.volume.cc = 30;
		osc3.volume.cc_amount = 0.15;
		osc3.unison_amount = 2;
		osc3.unison_detune.value = 0.05;
		osc3.env = { 0.0005, 0, 1, 0.07 };

		OscilatorEntity &osc4 = preset.oscilators.at(4);
		osc4.waveform = AnalogWaveForm::SAW_DOWN_WAVE;
		osc4.volume.value = 0;
		osc4.volume.cc = 30;
		osc4.volume.cc_amount = 0.15;
		osc4.unison_amount = 2;
		osc4.unison_detune.value = 0.05;
		osc4.semi = 12;
		osc4.env = { 0.0005, 0, 1, 0.07 };

		preset.lfo_count = 0;
		preset.mod_env_count = 0;
		preset.osc_count = 4;
	}
		break;
	}
}

AnalogSynth::AnalogSynth() {
	for (size_t i = 0; i < parts.size(); ++i) {
		parts[i].preset = &preset;
		parts[i].part = i;
	}
}

static inline double apply_modulation(const FixedScale &scale,
		PropertyModulation &mod, std::array<double, ANALOG_PART_COUNT> &env_val,
		std::array<double, ANALOG_PART_COUNT> &lfo_val,
		std::array<double, ANALOG_CONTROL_COUNT> &controls, double velocity) {
	double prog = mod.value;
	prog += env_val[mod.mod_env] * mod.mod_env_amount
			+ lfo_val[mod.lfo] * mod.lfo_amount
			+ controls[mod.cc] * mod.cc_amount + velocity * mod.velocity_amount;
	prog = fmin(fmax(prog, 0.0), 1.0);
	return scale.value(prog);
}

void AnalogSynth::process_note(std::array<double, OUTPUT_CHANNELS> &channels,
		SampleInfo &info, TriggeredNote &note, KeyboardEnvironment &env,
		size_t note_index) {
	env_val = { };
	//Reset amps
	bool reset_amps = amp_finished(info, note, env, note_index); //TODO maybe use press note event
	//Mod Envs
	for (size_t i = 0; i < preset.mod_env_count; ++i) {
		ModEnvelopeEntity &mod_env = preset.mod_envs[i];
		size_t index = ENV_INDEX(note_index, i);
		if (reset_amps) {
			mod_envs[index].reset();
		}

		double volume = apply_modulation(VOLUME_SCALE, mod_env.volume, env_val,
				lfo_val, controls, note.velocity);
		env_val[i] =
				mod_envs.at(note_index + i * SOUND_ENGINE_POLYPHONY).amplitude(
						mod_env.env, info.time_step, note.pressed, env.sustain)
						* volume;
	}
	//Synthesize
	double lsample = 0;
	double rsample = 0;
	for (size_t i = 0; i < preset.osc_count; ++i) {
		OscilatorEntity &osc = preset.oscilators[i];
		//Frequency
		double freq = note.freq;
		//FM
		const size_t fm_count = osc.fm.size();
		for (size_t i = 0; i < fm_count; ++i) {
			FrequencyModulatotion &fm = osc.fm[i];
			if (fm.fm_amount) {
				freq += modulators.at(OSC_INDEX(note_index, fm.modulator))
						* fm.fm_amount;
			}
		}
		double pitch = apply_modulation(PITCH_SCALE, osc.pitch, env_val,
				lfo_mod, controls, note.velocity);
		if (osc.semi || pitch) {
			freq = note_to_freq(note.note + osc.semi + pitch);
		}
		freq *= env.pitch_bend * osc.transpose;

		AnalogOscilatorData data = { osc.waveform, osc.analog, osc.sync };
		AnalogOscilatorBankData bdata = { 0.1, osc.unison_amount };
		size_t index = OSC_INDEX(note_index, i);
		//Only on note start
		if (reset_amps) {
			amp_envs[index].reset();
			if (osc.reset) {
				oscilators.reset(index);
			} else if (osc.randomize) {
				oscilators.randomize(index);
			}
		}

		//Apply modulation
		double volume = apply_modulation(VOLUME_SCALE, osc.volume, env_val,
				lfo_val, controls, note.velocity)
				* amp_envs.at(index).amplitude(osc.env, info.time_step,
						note.pressed, env.sustain);
		data.sync_mul = apply_modulation(SYNC_SCALE, osc.sync_mul, env_val,
				lfo_val, controls, note.velocity);
		data.pulse_width = apply_modulation(PULSE_WIDTH_SCALE, osc.pulse_width,
				env_val, lfo_val, controls, note.velocity);
		bdata.unison_detune = apply_modulation(UNISON_DETUNE_SCALE,
				osc.unison_detune, env_val, lfo_val, controls, note.velocity);

		//Signal
		AnalogOscilatorSignal sig = oscilators.signal(freq, info.time_step,
				index, data, bdata);
		double signal = sig.carrier;
		modulators[index] = sig.modulator * volume;
		if (osc.audible) {
			//Filter
			if (osc.filter) {
				FilterData filter { osc.filter_type };
				filter.cutoff = apply_modulation(FILTER_CUTOFF_SCALE,
						osc.filter_cutoff, env_val, lfo_val, controls,
						note.velocity);
				filter.resonance = apply_modulation(FILTER_RESONANCE_SCALE,
						osc.filter_resonance, env_val, lfo_val, controls,
						note.velocity);

				if (osc.filter_kb_track && filter.cutoff != 1) {
					double cutoff = factor_to_cutoff(filter.cutoff,
							info.time_step);
					//KB track
					cutoff *= 1
							+ ((double) note.note - 36) / 12.0
									* osc.filter_kb_track;
					filter.cutoff = cutoff_to_factor(cutoff, info.time_step);
				}

				signal = filters[note_index + i * SOUND_ENGINE_POLYPHONY].apply(
						filter, signal, info.time_step);
			}
			signal *= volume;
			//Pan
			double panning = apply_modulation(PANNING_SCALE, osc.panning,
					env_val, lfo_val, controls, note.velocity);
			lsample += signal * (1 - fmax(0, panning));
			rsample += signal * (1 - fmax(0, -panning));
		}
	}

	//Play
	for (size_t i = 0; i < channels.size(); ++i) {
		if (i % 2 == 0) {
			channels[i] += lsample;
		} else {
			channels[i] += rsample;
		}
	}
}

void AnalogSynth::process_note_sample(
		std::array<double, OUTPUT_CHANNELS> &channels, SampleInfo &info,
		TriggeredNote &note, KeyboardEnvironment &env, size_t note_index) {
	if (!preset.mono) {
		process_note(channels, info, note, env, note_index);
	}
}

void AnalogSynth::process_sample(std::array<double, OUTPUT_CHANNELS> &channels,
		SampleInfo &info, KeyboardEnvironment &env, EngineStatus &status) {
	//Mono
	if (preset.mono && status.latest_note) {
		unsigned int note = status.latest_note->note;
		//Update portamendo
		if (note != last_note) {
			//Reset envs to attack
			if (!preset.legato) {
				for (size_t i = 0; i < preset.mod_env_count; ++i) {
					//Mod env
					size_t index = ENV_INDEX(0, i);	//Updating every amp might be a bug source
					mod_envs[index].phase = ATTACK;
				}
				for (size_t i = 0; i < preset.osc_count; ++i) {
					//Amp env
					size_t index = OSC_INDEX(0, i);
					amp_envs[index].phase = ATTACK;
				}
			}
			note_port.set(note, info.time, first_port ? 0 : preset.portamendo);
		}
		last_note = note;
		first_port = false;
		double pitch = note_port.get(info.time);
		KeyboardEnvironment e = env;
		e.pitch_bend *= note_to_freq_transpose(
				pitch - status.latest_note->note);

		process_note(channels, info, *status.latest_note, e, 0);
	}

	//Delay lines
	if (preset.delay_mix) {
		//Get samples
		double lsample = 0;
		double rsample = 0;
		for (size_t i = 0; i < channels.size(); ++i) {
			if (i % 2 == 0) {
				lsample += channels[i];
			} else {
				rsample += channels[i];
			}
		}
		//Apply delay
		ldelay.add_isample(lsample, preset.delay_time * info.sample_rate);
		rdelay.add_isample(rsample, preset.delay_time * info.sample_rate);

		double ldsample = ldelay.process();
		double rdsample = rdelay.process();

		ldelay.add_sample(ldsample * preset.delay_feedback,
				preset.delay_time * info.sample_rate - 1);
		rdelay.add_sample(rdsample * preset.delay_feedback,
				preset.delay_time * info.sample_rate - 1);
		//Play delay
		lsample *= 1 - (fmax(0, preset.delay_mix - 0.5) * 2);
		rsample *= 1 - (fmax(0, preset.delay_mix - 0.5) * 2);

		lsample += ldsample * fmin(0.5, preset.delay_mix) * 2;
		rsample += rdsample * fmin(0.5, preset.delay_mix) * 2;

		for (size_t i = 0; i < channels.size(); ++i) {
			if (i % 2 == 0) {
				channels[i] += lsample;
			} else {
				channels[i] += rsample;
			}
		}
	}
	//Move LFOs
	//TODO move before notes
	lfo_val = { };
	lfo_mod = { };
	for (size_t i = 0; i < preset.lfo_count; ++i) {
		LFOEntity &lfo = preset.lfos[i];
		double volume = apply_modulation(VOLUME_SCALE, lfo.volume, env_val,
				lfo_val, controls, 0);
		AnalogOscilatorData d = { lfo.waveform };
		AnalogOscilatorSignal sig = lfos[i].signal(lfo.freq, info.time_step, d);
		lfo_val.at(i) = sig.carrier * volume;
		lfo_mod.at(i) = sig.modulator * volume;
	}
}

void AnalogSynth::control_change(unsigned int control, unsigned int value) {
	controls.at(control) = value / 127.0;
}

bool AnalogSynth::note_finished(SampleInfo &info, TriggeredNote &note,
		KeyboardEnvironment &env, size_t note_index) {
	//Mono notes
	if (preset.mono) {	//TODO not very easy to read/side effects
		note_index = 0;
	}
	return !note.pressed && amp_finished(info, note, env, note_index);
}

bool AnalogSynth::amp_finished(SampleInfo &info, TriggeredNote &note,
		KeyboardEnvironment &env, size_t note_index) {
	bool finished = true;
	for (size_t i = 0; i < preset.osc_count && finished; ++i) {
		size_t index = OSC_INDEX(note_index, i);
		finished = amp_envs[index].is_finished();
	}
	return finished;
}

void AnalogSynth::set(size_t prop, PropertyValue value, size_t sub_prop) {
	switch ((SynthProperty) prop) {
	case SynthProperty::pSynthOscCount:
		preset.osc_count = value.ival;
		break;
	case SynthProperty::pSynthModEnvCount:
		preset.mod_env_count = value.ival;
		break;
	case SynthProperty::pSynthLFOCount:
		preset.lfo_count = value.ival;
		break;
	case SynthProperty::pSynthMono:
		preset.mono = value.bval;
		break;
	case SynthProperty::pSynthLegato:
		preset.legato = value.bval;
		break;
	case SynthProperty::pSynthPortamendo:
		preset.portamendo = value.dval;
		break;
	case SynthProperty::pSynthDelayMix:
		preset.delay_mix = value.dval;
		break;
	case SynthProperty::pSynthDelayTime:
		preset.delay_time = value.dval;
		break;
	case SynthProperty::pSynthDelayFeedback:
		preset.delay_feedback = value.dval;
		break;
	}
}

PropertyValue AnalogSynth::get(size_t prop, size_t sub_prop) {
	PropertyValue value;
	switch ((SynthProperty) prop) {
	case SynthProperty::pSynthOscCount:
		value.ival = preset.osc_count;
		break;
	case SynthProperty::pSynthModEnvCount:
		value.ival = preset.mod_env_count;
		break;
	case SynthProperty::pSynthLFOCount:
		value.ival = preset.lfo_count;
		break;
	case SynthProperty::pSynthMono:
		value.bval = preset.mono;
		break;
	case SynthProperty::pSynthLegato:
		value.bval = preset.legato;
		break;
	case SynthProperty::pSynthPortamendo:
		value.dval = preset.portamendo;
		break;
	case SynthProperty::pSynthDelayMix:
		value.dval = preset.delay_mix;
		break;
	case SynthProperty::pSynthDelayTime:
		value.dval = preset.delay_time;
		break;
	case SynthProperty::pSynthDelayFeedback:
		value.dval = preset.delay_feedback;
		break;
	}
	return value;
}

AnalogSynth::~AnalogSynth() {

}

template<>
std::string get_engine_name<AnalogSynth>() {
	return "Analog Synthesizer";
}

PropertyValue get_mod_prop(PropertyModulation &mod,
		SynthModulationProperty prop) {
	PropertyValue val;
	switch (prop) {
	case SynthModulationProperty::pModValue:
		val.dval = mod.value;
		break;
	case SynthModulationProperty::pModModEnv:
		val.ival = mod.mod_env;
		break;
	case SynthModulationProperty::pModModEnvAmount:
		val.dval = mod.mod_env_amount;
		break;
	case SynthModulationProperty::pModLFO:
		val.ival = mod.lfo;
		break;
	case SynthModulationProperty::pModLFOAmount:
		val.dval = mod.lfo_amount;
		break;
	case SynthModulationProperty::pModVelocityAmount:
		val.dval = mod.velocity_amount;
		break;
	case SynthModulationProperty::pModCC:
		val.ival = mod.cc;
		break;
	case SynthModulationProperty::pModCCAmount:
		val.dval = mod.cc_amount;
		break;
	}
	return val;
}

void set_mod_prop(PropertyModulation &mod, SynthModulationProperty prop,
		PropertyValue val) {
	switch (prop) {
	case SynthModulationProperty::pModValue:
		mod.value = val.dval;
		break;
	case SynthModulationProperty::pModModEnv:
		mod.mod_env = val.ival;
		break;
	case SynthModulationProperty::pModModEnvAmount:
		mod.mod_env_amount = val.dval;
		break;
	case SynthModulationProperty::pModLFO:
		mod.lfo = val.ival;
		break;
	case SynthModulationProperty::pModLFOAmount:
		mod.lfo_amount = val.dval;
		break;
	case SynthModulationProperty::pModVelocityAmount:
		mod.velocity_amount = val.dval;
		break;
	case SynthModulationProperty::pModCC:
		mod.cc = val.ival;
		break;
	case SynthModulationProperty::pModCCAmount:
		mod.cc_amount = val.dval;
		break;
	}
}

//SynthPartPropertyHolder
PropertyValue SynthPartPropertyHolder::get(size_t prop, size_t sub_prop) {
	PropertyValue val;
	OscilatorEntity &osc = preset->oscilators[this->part];
	ModEnvelopeEntity &env = preset->mod_envs[this->part];
	LFOEntity &lfo = preset->lfos[this->part];
	switch ((SynthPartProperty) prop) {
	case SynthPartProperty::pSynthOscAudible:
		val.bval = osc.audible;
		break;
	case SynthPartProperty::pSynthOscAttack:
		val.dval = osc.env.attack;
		break;
	case SynthPartProperty::pSynthOscDecay:
		val.dval = osc.env.decay;
		break;
	case SynthPartProperty::pSynthOscSustain:
		val.dval = osc.env.sustain;
		break;
	case SynthPartProperty::pSynthOscRelease:
		val.dval = osc.env.release;
		break;
	case SynthPartProperty::pSynthOscWaveForm:
		val.ival = osc.waveform;
		break;
	case SynthPartProperty::pSynthOscAnalog:
		val.bval = osc.analog;
		break;
	case SynthPartProperty::pSynthOscSync:
		val.bval = osc.sync;
		break;
	case SynthPartProperty::pSynthOscReset:
		val.bval = osc.reset;
		break;
	case SynthPartProperty::pSynthOscRandomize:
		val.bval = osc.randomize;
		break;
	case SynthPartProperty::pSynthOscUnisonAmount:
		val.ival = osc.unison_amount;
		break;
	case SynthPartProperty::pSynthOscVolume:
		val = get_mod_prop(osc.volume, (SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscSyncMul:
		val = get_mod_prop(osc.sync_mul, (SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscPulseWidth:
		val = get_mod_prop(osc.pulse_width, (SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscUnisonDetune:
		val = get_mod_prop(osc.unison_detune,
				(SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscPanning:
		val = get_mod_prop(osc.panning, (SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscSemi:
		val.dval = osc.semi;
		break;
	case SynthPartProperty::pSynthOscTranspose:
		val.dval = osc.transpose;
		break;
	case SynthPartProperty::pSynthOscPitch:
		val = get_mod_prop(osc.pitch, (SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscFilter:
		val.bval = osc.filter;
		break;
	case SynthPartProperty::pSynthOscFilterType:
		val.ival = osc.filter_type;
		break;
	case SynthPartProperty::pSynthOscFilterCutoff:
		val = get_mod_prop(osc.filter_cutoff,
				(SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscFilterResonance:
		val = get_mod_prop(osc.filter_resonance,
				(SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscFilterKBTrack:
		val.dval = osc.filter_kb_track;
		break;
	case SynthPartProperty::pSynthOscFilterKBTrackNote:
		val.ival = osc.filter_kb_track_note;
		break;

	case SynthPartProperty::pSynthEnvAttack:
		val.dval = env.env.attack;
		break;
	case SynthPartProperty::pSynthEnvDecay:
		val.dval = env.env.decay;
		break;
	case SynthPartProperty::pSynthEnvSustain:
		val.dval = env.env.sustain;
		break;
	case SynthPartProperty::pSynthEnvRelease:
		val.dval = env.env.release;
		break;
	case SynthPartProperty::pSynthEnvVolume:
		val = get_mod_prop(env.volume, (SynthModulationProperty) sub_prop);
		break;

	case SynthPartProperty::pSynthLFOFrequency:
		val.dval = lfo.freq;
		break;
	case SynthPartProperty::pSynthLFOWaveForm:
		val.ival = lfo.waveform;
		break;
	case SynthPartProperty::pSynthLFOVolume:
		val = get_mod_prop(lfo.volume, (SynthModulationProperty) sub_prop);
		break;
	}
	return val;
}

void SynthPartPropertyHolder::set(size_t prop, PropertyValue val,
		size_t sub_prop) {
	OscilatorEntity &osc = preset->oscilators[this->part];
	ModEnvelopeEntity &env = preset->mod_envs[this->part];
	LFOEntity &lfo = preset->lfos[this->part];
	switch ((SynthPartProperty) prop) {
	case SynthPartProperty::pSynthOscAudible:
		osc.audible = val.bval;
		break;
	case SynthPartProperty::pSynthOscAttack:
		osc.env.attack = val.dval;
		break;
	case SynthPartProperty::pSynthOscDecay:
		osc.env.decay = val.dval;
		break;
	case SynthPartProperty::pSynthOscSustain:
		osc.env.sustain = val.dval;
		break;
	case SynthPartProperty::pSynthOscRelease:
		osc.env.release = val.dval;
		break;
	case SynthPartProperty::pSynthOscWaveForm:
		osc.waveform = (AnalogWaveForm) val.ival;
		break;
	case SynthPartProperty::pSynthOscAnalog:
		osc.analog = val.bval;
		break;
	case SynthPartProperty::pSynthOscSync:
		osc.sync = val.bval;
		break;
	case SynthPartProperty::pSynthOscReset:
		osc.reset = val.bval;
		break;
	case SynthPartProperty::pSynthOscRandomize:
		osc.randomize = val.bval;
		break;
	case SynthPartProperty::pSynthOscUnisonAmount:
		osc.unison_amount = val.ival;
		break;
	case SynthPartProperty::pSynthOscVolume:
		set_mod_prop(osc.volume, (SynthModulationProperty) sub_prop, val);
		break;
	case SynthPartProperty::pSynthOscSyncMul:
		set_mod_prop(osc.sync_mul, (SynthModulationProperty) sub_prop, val);
		break;
	case SynthPartProperty::pSynthOscPulseWidth:
		set_mod_prop(osc.pulse_width, (SynthModulationProperty) sub_prop, val);
		break;
	case SynthPartProperty::pSynthOscUnisonDetune:
		set_mod_prop(osc.unison_detune, (SynthModulationProperty) sub_prop,
				val);
		break;
	case SynthPartProperty::pSynthOscPanning:
		set_mod_prop(osc.panning, (SynthModulationProperty) sub_prop, val);
		break;
	case SynthPartProperty::pSynthOscSemi:
		osc.semi = val.dval;
		break;
	case SynthPartProperty::pSynthOscTranspose:
		osc.transpose = val.dval;
		break;
	case SynthPartProperty::pSynthOscPitch:
		set_mod_prop(osc.pitch, (SynthModulationProperty) sub_prop, val);
		break;
	case SynthPartProperty::pSynthOscFilter:
		osc.filter = val.bval;
		break;
	case SynthPartProperty::pSynthOscFilterType:
		osc.filter_type = (FilterType) val.ival;
		break;
	case SynthPartProperty::pSynthOscFilterCutoff:
		set_mod_prop(osc.filter_cutoff, (SynthModulationProperty) sub_prop,
				val);
		break;
	case SynthPartProperty::pSynthOscFilterResonance:
		set_mod_prop(osc.filter_resonance, (SynthModulationProperty) sub_prop,
				val);
		break;
	case SynthPartProperty::pSynthOscFilterKBTrack:
		osc.filter_kb_track = val.dval;
		break;
	case SynthPartProperty::pSynthOscFilterKBTrackNote:
		osc.filter_kb_track_note = val.ival;
		break;

	case SynthPartProperty::pSynthEnvAttack:
		env.env.attack = val.dval;
		break;
	case SynthPartProperty::pSynthEnvDecay:
		env.env.decay = val.dval;
		break;
	case SynthPartProperty::pSynthEnvSustain:
		env.env.sustain = val.dval;
		break;
	case SynthPartProperty::pSynthEnvRelease:
		env.env.release = val.dval;
		break;
	case SynthPartProperty::pSynthEnvVolume:
		set_mod_prop(env.volume, (SynthModulationProperty) sub_prop, val);
		break;

	case SynthPartProperty::pSynthLFOFrequency:
		lfo.freq = val.dval;
		break;
	case SynthPartProperty::pSynthLFOWaveForm:
		lfo.waveform = (AnalogWaveForm) val.ival;
		break;
	case SynthPartProperty::pSynthLFOVolume:
		set_mod_prop(lfo.volume, (SynthModulationProperty) sub_prop, val);
		break;
	}
}

SynthPartPropertyHolder::SynthPartPropertyHolder(AnalogSynthPreset *p, size_t i) :
		preset(p), part(i) {

}

void SynthPartPropertyHolder::update_properties() {
	OscilatorEntity &osc = preset->oscilators[this->part];
	ModEnvelopeEntity &env = preset->mod_envs[this->part];
	LFOEntity &lfo = preset->lfos[this->part];

	PropertyHolder::submit_change(SynthPartProperty::pSynthOscAudible,
			osc.audible);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscAttack,
			osc.env.attack);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscDecay,
			osc.env.decay);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscSustain,
			osc.env.sustain);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscRelease,
			osc.env.release);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscWaveForm,
			osc.waveform);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscAnalog,
			osc.analog);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscSync, osc.sync);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscReset, osc.reset);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscRandomize,
			osc.randomize);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscUnisonAmount,
			(int) osc.unison_amount);

	submit_change(SynthPartProperty::pSynthOscVolume, osc.volume);
	submit_change(SynthPartProperty::pSynthOscSyncMul, osc.sync_mul);
	submit_change(SynthPartProperty::pSynthOscPulseWidth, osc.pulse_width);
	submit_change(SynthPartProperty::pSynthOscUnisonDetune, osc.unison_detune);
	submit_change(SynthPartProperty::pSynthOscPanning, osc.panning);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscSemi, osc.semi);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscTranspose,
			osc.transpose);
	submit_change(SynthPartProperty::pSynthOscPitch, osc.pitch);

	PropertyHolder::submit_change(SynthPartProperty::pSynthOscFilter,
			osc.filter);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscFilterType,
			osc.filter_type);
	submit_change(SynthPartProperty::pSynthOscFilterCutoff, osc.filter_cutoff);
	submit_change(SynthPartProperty::pSynthOscFilterResonance,
			osc.filter_resonance);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscFilterKBTrack,
			osc.filter_kb_track);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscFilterKBTrackNote,
			(int) osc.filter_kb_track_note);

	PropertyHolder::submit_change(SynthPartProperty::pSynthEnvAttack,
			env.env.attack);
	PropertyHolder::submit_change(SynthPartProperty::pSynthEnvDecay,
			env.env.decay);
	PropertyHolder::submit_change(SynthPartProperty::pSynthEnvSustain,
			env.env.sustain);
	PropertyHolder::submit_change(SynthPartProperty::pSynthEnvRelease,
			env.env.release);
	submit_change(SynthPartProperty::pSynthEnvVolume, env.volume);

	PropertyHolder::submit_change(SynthPartProperty::pSynthLFOFrequency,
			lfo.freq);
	PropertyHolder::submit_change(SynthPartProperty::pSynthLFOWaveForm,
			(int) lfo.waveform);
	submit_change(SynthPartProperty::pSynthEnvVolume, lfo.volume);
}

void AnalogSynth::update_properties() {
	submit_change(SynthProperty::pSynthOscCount, (int) preset.osc_count);
	submit_change(SynthProperty::pSynthModEnvCount, (int) preset.mod_env_count);
	submit_change(SynthProperty::pSynthLFOCount, (int) preset.lfo_count);
	submit_change(SynthProperty::pSynthMono, preset.mono);
	submit_change(SynthProperty::pSynthLegato, preset.legato);
	submit_change(SynthProperty::pSynthPortamendo, preset.portamendo);
	submit_change(SynthProperty::pSynthDelayMix, preset.delay_mix);
	submit_change(SynthProperty::pSynthDelayTime, preset.delay_time);
	submit_change(SynthProperty::pSynthDelayFeedback, preset.delay_feedback);
}
