/*
 * asynth.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: jojo
 */
#include "asynth.h"
#include <cmath>
#include <iostream>

#define OSC_INDEX(note_index,i) (note_index + i * ANALOG_SYNTH_POLYPHONY)
#define ENV_INDEX(note_index,i) (note_index + i * ANALOG_SYNTH_POLYPHONY)

void apply_preset(SynthFactoryPreset type, AnalogSynthPreset &preset) {
	/*
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

		preset.lfo_count = 0;
		preset.mod_env_count = 1;
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
		osc1.fm.at(1) = 1;
		osc1.env = { 0, 0.2, 0, 0.003 };

		OscilatorEntity &osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::SINE_WAVE;
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
		osc1.fm.at(1) = 0.13;
		osc1.env = { 0.0005, 0.6, 0.6, 0.2 };

		OscilatorEntity &osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::SINE_WAVE;
		osc2.volume.value = 0.2;
		osc2.volume.velocity_amount = 0.3;
		osc2.env = { 0.0005, 1.8, 0.0, 0.1 };

		OscilatorEntity &osc3 = preset.oscilators.at(2);
		osc3.waveform = AnalogWaveForm::SINE_WAVE;
		osc3.volume.value = 0.0;
		osc3.volume.velocity_amount = 1;
		osc3.transpose = 10;
		osc3.audible = false;
		osc3.fm.at(3) = 0.25;
		osc3.env = { 0.0005, 0.2, 0.4, 0.2 };

		OscilatorEntity &osc4 = preset.oscilators.at(3);
		osc4.waveform = AnalogWaveForm::SINE_WAVE;
		osc4.volume.value = 0.5;
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
		osc1.fm.at(1) = 1;
		osc1.volume.value = 0.12;
		osc1.env = { 0.0005, 0.12, 0.0, 0.03 };

		OscilatorEntity &osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::SINE_WAVE;
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
	}*/
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

void AnalogSynth::process_note(double& lsample, double& rsample,
		SampleInfo &info, AnalogSynthVoice &note, KeyboardEnvironment &env,
		size_t note_index) {
	//Mod Envs
	for (size_t i = 0; i < preset.mod_env_count; ++i) {
		ModEnvelopeEntity &mod_env = preset.mod_envs[i];
		double volume = apply_modulation(VOLUME_SCALE, mod_env.volume, env_val, lfo_val, controls, note.velocity);
		env_val[i] = note.parts[i].mod_env.amplitude(mod_env.env, info.time_step, note.pressed, env.sustain)* volume;
	}
	//Synthesize
	size_t i = 0;
	for (size_t j = 0; j < preset.op_count; ++j) {
		OperatorEntity& op = preset.operators[j];
		AnalogSynthPart& op_part = note.parts[j];
		const size_t max = std::min(i + op.oscilator_count, (size_t) ANALOG_PART_COUNT);
		//FM
		double fm = op_part.fm * PI2;
		op_part.fm = 0;
		bool modulates = false;
		for (size_t j = 0; j < ANALOG_PART_COUNT; ++j) {
			if (op.fm[j]) {
				modulates = true;
				break;
			}
		}
		//Oscilators
		double carrier = 0;
		double modulator = 0;
		for (; i < max; ++i) {
			AnalogSynthPart& part = note.parts[i];
			OscilatorEntity &osc = preset.oscilators[i];
			//Frequency
			double freq = note.freq;
			//FM
			freq += fm;
			double pitch = apply_modulation(PITCH_SCALE, osc.pitch, env_val,
					lfo_mod, controls, note.velocity);
			if (osc.semi || pitch) {
				freq = note_to_freq(note.note + osc.semi + pitch);
			}
			freq *= env.pitch_bend * osc.transpose;

			AnalogOscilatorData data = { osc.waveform, osc.analog, osc.sync };
			AnalogOscilatorBankData bdata = { 0.1, osc.unison_amount };

			//Apply modulation
			double volume = apply_modulation(VOLUME_SCALE, osc.volume, env_val,
				lfo_val, controls, note.velocity);
			data.sync_mul = apply_modulation(SYNC_SCALE, osc.sync_mul, env_val,
					lfo_val, controls, note.velocity);
			data.pulse_width = apply_modulation(PULSE_WIDTH_SCALE, osc.pulse_width,
					env_val, lfo_val, controls, note.velocity);
			bdata.unison_detune = apply_modulation(UNISON_DETUNE_SCALE,
					osc.unison_detune, env_val, lfo_val, controls, note.velocity);

			AnalogOscilatorSignal sig = part.oscilator.signal(freq, info.time_step, data, bdata, op.audible, modulates);
			carrier += sig.carrier * volume;
			modulator += sig.modulator * volume;
		}
		//Post processing
		//Volume
		double volume = apply_modulation(VOLUME_SCALE, op.volume, env_val,
						lfo_val, controls, note.velocity) * op_part.amp_env.amplitude(op.env, info.time_step,
						note.pressed, env.sustain);
		//Frequency modulate others
		if (modulates) {
			modulator *= volume;
			for (size_t i = 0; i < ANALOG_PART_COUNT; ++i) {
				note.parts[i].fm += modulator * op.fm[i];
			}
		}
		//Playback
		if (op.audible) {
			//Filter
			if (op.filter) {
				FilterData filter { op.filter_type };
				filter.cutoff = scale_cutoff(apply_modulation(FILTER_CUTOFF_SCALE,
						op.filter_cutoff, env_val, lfo_val, controls,
						note.velocity)); //TODO optimize
				filter.resonance = apply_modulation(FILTER_RESONANCE_SCALE,
						op.filter_resonance, env_val, lfo_val, controls,
						note.velocity);

				if (op.filter_kb_track) {
					double cutoff = filter.cutoff;
					//KB track
					cutoff *= 1 + ((double) note.note - 36) / 12.0 * op.filter_kb_track;
					filter.cutoff = cutoff;
				}

				carrier = op_part.filter.apply(filter, carrier, info.time_step);
			}
			carrier *= volume;
			//Pan
			double panning = apply_modulation(PANNING_SCALE, op.panning,
					env_val, lfo_val, controls, note.velocity);
			//Playback
			lsample += carrier * (1 - fmax(0, panning));
			rsample += carrier * (1 - fmax(0, -panning));
		}
	}
}

void AnalogSynth::process_note_sample(
		double& lsample, double& rsample, SampleInfo &info,
		AnalogSynthVoice &note, KeyboardEnvironment &env, size_t note_index) {
	if (!preset.mono) {
		process_note(lsample, rsample, info, note, env, note_index);
	}
}

void AnalogSynth::process_sample(double& lsample, double& rsample,
		SampleInfo &info, KeyboardEnvironment &env, EngineStatus &status) {
	//Mono
	if (preset.mono && status.pressed_notes) {
		AnalogSynthVoice& voice = this->note.note[status.latest_note_index];
		//Update portamendo
		if (voice.note != last_note) {
			//Reset envs to attack
			if (!preset.legato) {
				for (size_t i = 0; i < preset.mod_env_count; ++i) {
					//Mod env
					voice.parts[i].mod_env.phase = ATTACK;
				}
				for (size_t i = 0; i < preset.op_count; ++i) {
					//Amp env
					voice.parts[i].mod_env.phase = ATTACK;
				}
			}
			note_port.set(voice.note, info.time, first_port ? 0 : preset.portamendo);
		}
		last_note = voice.note;
		first_port = false;
		double pitch = note_port.get(info.time);
		KeyboardEnvironment e = env;
		e.pitch_bend *= note_to_freq_transpose(
				pitch - voice.note);

		process_note(lsample, rsample, info, voice, e, 0);
	}

	//Delay lines
	if (preset.delay_mix) {
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
		lfos[i].process(lfo.freq, info.time_step, d);
		lfo_val[i] = lfos[i].carrier(lfo.freq, info.time_step, d) * volume;
		lfo_mod[i] = lfos[i].modulator(lfo.freq, info.time_step, d) * volume;
	}
}

void AnalogSynth::control_change(unsigned int control, unsigned int value) {
	controls[control] = value / 127.0;
}

bool AnalogSynth::note_finished(SampleInfo &info, AnalogSynthVoice &note,
		KeyboardEnvironment &env, size_t note_index) {
	//Mono notes
	if (preset.mono) {	//TODO not very easy to read/side effects
		note_index = 0;
	}
	return !note.pressed && amp_finished(info, note, env, note_index);
}

void AnalogSynth::press_note(SampleInfo& info, unsigned int note, double velocity) {
	AnalogSynthVoice& voice = this->note.note[this->note.press_note(info, note, velocity)];
	for (size_t i = 0; i < preset.mod_env_count; ++i) {
		voice.parts[i].mod_env.reset();
	}
	size_t j = 0;
	for (size_t i = 0; i < preset.op_count; ++i) {
		OperatorEntity& op = preset.operators[i];
		voice.parts[i].amp_env.reset();
		size_t max = std::max(j + op.oscilator_count, (size_t) ANALOG_PART_COUNT);
		for (; j < max; ++j) {
			OscilatorEntity& osc = preset.oscilators[j];
			if (osc.reset) {
				voice.parts[j].oscilator.reset();
			} else if (osc.randomize) {
				voice.parts[j].oscilator.randomize();
			}
		}
	}
}

void AnalogSynth::release_note(SampleInfo& info, unsigned int note) {
	BaseSoundEngine::release_note(info, note);
}

bool AnalogSynth::amp_finished(SampleInfo &info, AnalogSynthVoice &note,
		KeyboardEnvironment &env, size_t note_index) {
	bool finished = true;
	for (size_t i = 0; i < preset.op_count && finished; ++i) {
		finished = note.parts[i].amp_env.is_finished();
	}
	return finished;
}

void AnalogSynth::set(size_t prop, PropertyValue value, size_t sub_prop) {
	switch ((SynthProperty) prop) {
	case SynthProperty::pSynthOpCount:
		preset.op_count = value.ival;
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
	case SynthProperty::pSynthOpCount:
		value.ival = preset.op_count;
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
	OperatorEntity &op = preset->operators[this->part];
	ModEnvelopeEntity &env = preset->mod_envs[this->part];
	LFOEntity &lfo = preset->lfos[this->part];
	switch ((SynthPartProperty) prop) {
	case SynthPartProperty::pSynthOpAudible:
		val.bval = op.audible;
		break;
	case SynthPartProperty::pSynthOpAttack:
		val.dval = op.env.attack;
		break;
	case SynthPartProperty::pSynthOpDecay:
		val.dval = op.env.decay;
		break;
	case SynthPartProperty::pSynthOpSustain:
		val.dval = op.env.sustain;
		break;
	case SynthPartProperty::pSynthOpRelease:
		val.dval = op.env.release;
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
	case SynthPartProperty::pSynthOpVolume:
		val = get_mod_prop(op.volume, (SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOpOscCount:
		val.ival = op.oscilator_count;
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
	case SynthPartProperty::pSynthOpPanning:
		val = get_mod_prop(op.panning, (SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOscSemi:
		val.ival = osc.semi;
		break;
	case SynthPartProperty::pSynthOscTranspose:
		val.dval = osc.transpose;
		break;
	case SynthPartProperty::pSynthOscPitch:
		val = get_mod_prop(osc.pitch, (SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOpFM:
		val.dval = op.fm[sub_prop];
		break;
	case SynthPartProperty::pSynthOpFilter:
		val.bval = op.filter;
		break;
	case SynthPartProperty::pSynthOpFilterType:
		val.ival = op.filter_type;
		break;
	case SynthPartProperty::pSynthOpFilterCutoff:
		val = get_mod_prop(op.filter_cutoff,
				(SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOpFilterResonance:
		val = get_mod_prop(op.filter_resonance,
				(SynthModulationProperty) sub_prop);
		break;
	case SynthPartProperty::pSynthOpFilterKBTrack:
		val.dval = op.filter_kb_track;
		break;
	case SynthPartProperty::pSynthOpFilterKBTrackNote:
		val.ival = op.filter_kb_track_note;
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
	OperatorEntity &op = preset->operators[this->part];
	ModEnvelopeEntity &env = preset->mod_envs[this->part];
	LFOEntity &lfo = preset->lfos[this->part];
	switch ((SynthPartProperty) prop) {
	case SynthPartProperty::pSynthOpAudible:
		op.audible = val.bval;
		break;
	case SynthPartProperty::pSynthOpAttack:
		op.env.attack = val.dval;
		break;
	case SynthPartProperty::pSynthOpDecay:
		op.env.decay = val.dval;
		break;
	case SynthPartProperty::pSynthOpSustain:
		op.env.sustain = val.dval;
		break;
	case SynthPartProperty::pSynthOpRelease:
		op.env.release = val.dval;
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
	case SynthPartProperty::pSynthOpVolume:
		set_mod_prop(op.volume, (SynthModulationProperty) sub_prop, val);
		break;
	case SynthPartProperty::pSynthOpOscCount:
		op.oscilator_count = val.ival;
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
	case SynthPartProperty::pSynthOpPanning:
		set_mod_prop(op.panning, (SynthModulationProperty) sub_prop, val);
		break;
	case SynthPartProperty::pSynthOscSemi:
		osc.semi = val.ival;
		break;
	case SynthPartProperty::pSynthOscTranspose:
		osc.transpose = val.dval;
		break;
	case SynthPartProperty::pSynthOscPitch:
		set_mod_prop(osc.pitch, (SynthModulationProperty) sub_prop, val);
		break;
	case SynthPartProperty::pSynthOpFM:
		op.fm[sub_prop] = val.dval;
		break;
	case SynthPartProperty::pSynthOpFilter:
		op.filter = val.bval;
		break;
	case SynthPartProperty::pSynthOpFilterType:
		op.filter_type = (FilterType) val.ival;
		break;
	case SynthPartProperty::pSynthOpFilterCutoff:
		set_mod_prop(op.filter_cutoff, (SynthModulationProperty) sub_prop,
				val);
		break;
	case SynthPartProperty::pSynthOpFilterResonance:
		set_mod_prop(op.filter_resonance, (SynthModulationProperty) sub_prop,
				val);
		break;
	case SynthPartProperty::pSynthOpFilterKBTrack:
		op.filter_kb_track = val.dval;
		break;
	case SynthPartProperty::pSynthOpFilterKBTrackNote:
		op.filter_kb_track_note = val.ival;
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
	OperatorEntity &op = preset->operators[this->part];
	ModEnvelopeEntity &env = preset->mod_envs[this->part];
	LFOEntity &lfo = preset->lfos[this->part];

	PropertyHolder::submit_change(SynthPartProperty::pSynthOpAudible,
			op.audible);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOpAttack,
			op.env.attack);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOpDecay,
			op.env.decay);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOpSustain,
			op.env.sustain);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOpRelease,
			op.env.release);
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
	submit_change(SynthPartProperty::pSynthOpVolume, op.volume);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOpOscCount, (int) op.oscilator_count);
	submit_change(SynthPartProperty::pSynthOscSyncMul, osc.sync_mul);
	submit_change(SynthPartProperty::pSynthOscPulseWidth, osc.pulse_width);
	submit_change(SynthPartProperty::pSynthOscUnisonDetune, osc.unison_detune);
	submit_change(SynthPartProperty::pSynthOpPanning, op.panning);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscSemi, osc.semi);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOscTranspose,
			osc.transpose);
	submit_change(SynthPartProperty::pSynthOscPitch, osc.pitch);

	for (size_t i = 0; i < ANALOG_PART_COUNT; ++i) {
		PropertyHolder::submit_change(SynthPartProperty::pSynthOpFM, op.fm[i], i);
	}

	PropertyHolder::submit_change(SynthPartProperty::pSynthOpFilter,
			op.filter);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOpFilterType,
			op.filter_type);
	submit_change(SynthPartProperty::pSynthOpFilterCutoff, op.filter_cutoff);
	submit_change(SynthPartProperty::pSynthOpFilterResonance,
			op.filter_resonance);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOpFilterKBTrack,
			op.filter_kb_track);
	PropertyHolder::submit_change(SynthPartProperty::pSynthOpFilterKBTrackNote,
			(int) op.filter_kb_track_note);

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
	submit_change(SynthPartProperty::pSynthLFOVolume, lfo.volume);
}

void AnalogSynth::update_properties() {
	submit_change(SynthProperty::pSynthOpCount, (int) preset.op_count);
	submit_change(SynthProperty::pSynthModEnvCount, (int) preset.mod_env_count);
	submit_change(SynthProperty::pSynthLFOCount, (int) preset.lfo_count);
	submit_change(SynthProperty::pSynthMono, preset.mono);
	submit_change(SynthProperty::pSynthLegato, preset.legato);
	submit_change(SynthProperty::pSynthPortamendo, preset.portamendo);
	submit_change(SynthProperty::pSynthDelayMix, preset.delay_mix);
	submit_change(SynthProperty::pSynthDelayTime, preset.delay_time);
	submit_change(SynthProperty::pSynthDelayFeedback, preset.delay_feedback);
}

void AnalogSynth::save_program(EngineProgram **prog) {
	AnalogSynthProgram* p = dynamic_cast<AnalogSynthProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new AnalogSynthProgram();
	}
	p->preset = preset;
	*prog = p;
}

void AnalogSynth::apply_program(EngineProgram *prog) {
	AnalogSynthProgram* p = dynamic_cast<AnalogSynthProgram*>(prog);
	//Create new
	if (p) {
		preset = p->preset;
	}
	else {
		preset = {};
	}
}

static boost::property_tree::ptree save_prop_mod(PropertyModulation mod) {
	boost::property_tree::ptree tree;
	tree.put("value", mod.value);
	tree.put("mod_env", mod.mod_env);
	tree.put("mod_env_amount", mod.mod_env_amount);
	tree.put("lfo", mod.lfo);
	tree.put("lfo_amount", mod.lfo_amount);
	tree.put("velocity_amount", mod.velocity_amount);
	tree.put("cc", mod.cc);
	tree.put("cc_amount", mod.cc_amount);
	return tree;
}


static PropertyModulation load_prop_mod(boost::property_tree::ptree tree) {
	PropertyModulation mod;
	mod.value = tree.get<double>("value", 0);
	mod.mod_env = tree.get<size_t>("mod_env", 0);
	mod.mod_env_amount = tree.get<double>("mod_env_amount", 0);
	mod.lfo = tree.get<size_t>("lfo", 0);
	mod.lfo_amount = tree.get<double>("lfo_amount", 0);
	mod.velocity_amount = tree.get<double>("velocity_amount", 0);
	mod.cc = tree.get<size_t>("cc", 1);
	mod.cc_amount = tree.get<double>("cc_amount", 0);
	return mod;
}

static PropertyModulation load_prop_mod(boost::property_tree::ptree parent, std::string path) {
	const auto& val = parent.get_child_optional(path);
	if (val) {
		return load_prop_mod(val.get());
	}
	return {};
}

static ADSREnvelopeData load_adsr(boost::property_tree::ptree tree) {
	ADSREnvelopeData data;

	data.attack = tree.get("attack", 0.0005);
	data.decay = tree.get("decay", 0.0);
	data.sustain = tree.get("sustain", 1.0);
	data.release = tree.get("release", 0.0005);

	return data;
}

static ADSREnvelopeData load_adsr(boost::property_tree::ptree parent, std::string path) {
	const auto& val = parent.get_child_optional(path);
	if (val) {
		return load_adsr(val.get());
	}
	return {0.0005, 0, 1, 0.0005};
}


static boost::property_tree::ptree save_adsr(ADSREnvelopeData data) {
	boost::property_tree::ptree tree;
	tree.put("attack", data.attack);
	tree.put("decay", data.decay);
	tree.put("sustain", data.sustain);
	tree.put("release", data.release);
	return tree;
}

void AnalogSynthProgram::load(boost::property_tree::ptree tree) {
	preset = {};
	//Global patch info
	preset.lfo_count = tree.get<size_t>("lfo_count", 0);
	preset.mod_env_count = tree.get<size_t>("mod_env_count", 0);
	preset.op_count = tree.get<size_t>("op_count", 1);

	preset.mono = tree.get<bool>("mono", false);
	preset.legato = tree.get<bool>("legato", false);
	preset.portamendo = tree.get<double>("portamendo", 0.0);

	preset.delay_time = tree.get<double>("delay_time", 0.0);
	preset.delay_feedback = tree.get<double>("delay_feedback", 0.0);
	preset.delay_mix = tree.get<double>("delay_mix", 0.0);

	//LFOs
	const auto& lfos = tree.get_child_optional("lfos");
	if (lfos) {
		size_t i = 0;
		for (pt::ptree::value_type& lfo : lfos.get()) {
			if (i >= ANALOG_PART_COUNT) {
				break;
			}
			preset.lfos[i].volume = load_prop_mod(lfo.second, "volume");
			preset.lfos[i].freq = lfo.second.get<double>("freq", 1);
			preset.lfos[i].waveform = (AnalogWaveForm) lfo.second.get<int>("waveform", 0);
			++i;
		}
	}

	//Envs
	const auto& envs = tree.get_child_optional("mod_envs");
	if (envs) {
		size_t i = 0;
		for (pt::ptree::value_type& env : envs.get()) {
			if (i >= ANALOG_PART_COUNT) {
				break;
			}
			preset.mod_envs[i].volume = load_prop_mod(env.second, "volume");
			preset.mod_envs[i].env = load_adsr(env.second, "env");
			++i;
		}
	}

	//Oscilators
	const auto& oscs = tree.get_child_optional("oscilators");
	if (oscs) {
		size_t i = 0;
		for (pt::ptree::value_type& o : oscs.get()) {
			if (i >= ANALOG_PART_COUNT) {
				break;
			}
			OscilatorEntity& osc = preset.oscilators[i];
			osc.waveform = (AnalogWaveForm) o.second.get<int>("waveform", 0);
			osc.analog = o.second.get<bool>("analog", true);
			osc.sync = o.second.get<bool>("sync", false);
			osc.reset = o.second.get<bool>("reset", false);
			osc.randomize = o.second.get<bool>("randomize", false);

			osc.unison_amount = o.second.get<size_t>("unison_amount", 0);
			osc.volume = load_prop_mod(o.second, "volume");
			osc.sync_mul = load_prop_mod(o.second, "sync_mul");
			osc.pulse_width = load_prop_mod(o.second, "pulse_width");
			osc.unison_detune = load_prop_mod(o.second, "unison_detune");

			osc.semi = o.second.get<int>("semi", 0);
			osc.transpose = o.second.get<double>("transpose", 0);
			osc.pitch = load_prop_mod(o.second, "pitch");
			++i;
		}
	}

	//Operators
	const auto& ops = tree.get_child_optional("operators");
	if (ops) {
		size_t i = 0;
		for (pt::ptree::value_type& o : ops.get()) {
			if (i >= ANALOG_PART_COUNT) {
				break;
			}

			OperatorEntity& op = preset.operators[i];

			op.audible = o.second.get<bool>("audible", true);
			op.env = load_adsr(o.second, "env");
			op.volume = load_prop_mod(o.second, "volume");
			op.panning = load_prop_mod(o.second, "panning");

			op.filter = o.second.get<bool>("filter", true);
			op.filter_type = (FilterType) o.second.get<int>("filter_type", 0);
			op.filter_cutoff = load_prop_mod(o.second, "filter_cutoff");
			op.filter_resonance = load_prop_mod(o.second, "filter_resonance");
			op.filter_kb_track = o.second.get<double>("filter_kb_track", 0);
			op.filter_kb_track_note = o.second.get<int>("filter_kb_track", 36);

			op.oscilator_count = o.second.get<size_t>("oscilator_count", 1);


			const auto& fm = tree.get_child_optional("operators");
			size_t k = 0;
			if (fm) {
				for (pt::ptree::value_type& f : fm.get()) {
					if (k >= ANALOG_PART_COUNT) {
						break;
					}

					op.fm[k] = f.second.get_value<double>(0.0);

					++k;
				}
			}

			++i;
		}
	}
}

boost::property_tree::ptree AnalogSynthProgram::save() {
	boost::property_tree::ptree tree;
	//Global patch info
	tree.put("lfo_count", preset.lfo_count);
	tree.put("mod_env_count", preset.mod_env_count);
	tree.put("op_count", preset.op_count);

	tree.put("mono", preset.mono);
	tree.put("legato", preset.legato);
	tree.put("portamendo", preset.portamendo);

	tree.put("delay_time", preset.delay_time);
	tree.put("delay_feedback", preset.delay_feedback);
	tree.put("delay_mix", preset.delay_mix);

	//LFOs
	for (size_t i = 0; i < preset.lfo_count; ++i) {
		boost::property_tree::ptree lfo;
		lfo.add_child("volume", save_prop_mod(preset.lfos[i].volume));
		lfo.put("freq", preset.lfos[i].freq);
		lfo.put("waveform", (int) preset.lfos[i].waveform);

		tree.add_child("lfos.lfo", lfo);
	}

	//Envs
	for (size_t i = 0; i < preset.mod_env_count; ++i) {
		boost::property_tree::ptree env;
		env.add_child("volume", save_prop_mod(preset.mod_envs[i].volume));
		env.add_child("env", save_adsr(preset.mod_envs[i].env));

		tree.add_child("mod_envs.mod_ens", env);
	}

	//Operators
	size_t osc_count = 0;
	for (size_t i = 0; i < preset.op_count; ++i) {
		OperatorEntity& op = preset.operators[i];
		//Oscilators
		for (size_t j = 0; j < op.oscilator_count && j + osc_count < ANALOG_PART_COUNT; ++j) {
			boost::property_tree::ptree o;
			OscilatorEntity& osc = preset.oscilators[osc_count + j];
			o.put("waveform", (int) osc.waveform);
			o.put("analog", osc.analog);
			o.put("sync", osc.sync);
			o.put("reset", osc.reset);
			o.put("randomize", osc.randomize);

			o.put("unison_amount", osc.unison_amount);
			o.add_child("volume", save_prop_mod(osc.volume));
			o.add_child("sync_mul", save_prop_mod(osc.sync_mul));
			o.add_child("pulse_width", save_prop_mod(osc.pulse_width));
			o.add_child("unison_detune", save_prop_mod(osc.unison_detune));

			o.put("semi", osc.semi);
			o.put("transpose", osc.transpose);
			o.add_child("pitch", save_prop_mod(osc.pitch));

			tree.add_child("oscilators.oscilator", o);
		}
		osc_count += op.oscilator_count;

		//Operator
		boost::property_tree::ptree o;
		o.put("audible", op.audible);
		o.add_child("env", save_adsr(op.env));
		o.add_child("volume", save_prop_mod(op.volume));
		o.add_child("panning", save_prop_mod(op.panning));

		o.put("filter", op.filter);
		o.put("filter_type", (int) op.filter_type);
		o.add_child("filter_cutoff", save_prop_mod(op.filter_cutoff));
		o.add_child("filter_resonance", save_prop_mod(op.filter_resonance));
		o.put("filter_kb_track", op.filter_kb_track);
		o.put("filter_kb_track_note", op.filter_kb_track_note);

		o.put("oscilator_count", op.oscilator_count);

		//FM
		for (size_t k = 0; k < ANALOG_PART_COUNT; ++k) {
			o.add("fm.amount", op.fm[k]);
		}

		tree.add_child("operators.operator", o);
	}

	return tree;
}
