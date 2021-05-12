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

}

static inline double apply_modulation(const FixedScale &scale,
		PropertyModulation &mod, std::array<double, ANALOG_PART_COUNT> &env_val,
		std::array<double, ANALOG_PART_COUNT> &lfo_val,
		std::array<double, ANALOG_CONTROL_COUNT> &controls, double velocity, double aftertouch) {
	double prog = mod.value;
	prog += env_val[mod.mod_env] * mod.mod_env_amount
			+ lfo_val[mod.lfo] * mod.lfo_amount
			+ controls[mod.cc] * mod.cc_amount + velocity * mod.velocity_amount + aftertouch * mod.aftertouch_amount;
	prog = fmin(fmax(prog, 0.0), 1.0);
	return scale.value(prog);
}

void AnalogSynth::process_note(double& lsample, double& rsample,
		SampleInfo &info, AnalogSynthVoice &note, KeyboardEnvironment &env) {
	//Mod Envs
	for (size_t i = 0; i < preset.mod_env_count; ++i) {
		ModEnvelopeEntity &mod_env = preset.mod_envs[i];
		double volume = apply_modulation(VOLUME_SCALE, mod_env.volume, env_val, lfo_val, controls, note.velocity, aftertouch);
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
			double freq = 0;
			if (osc.fixed_freq) {
				freq = note_to_freq(osc.note);
				//FM
				freq += fm;
				double pitch = apply_modulation(PITCH_SCALE, osc.pitch, env_val,
					lfo_mod, controls, note.velocity, aftertouch);
				if (pitch) {
					freq *= note_to_freq_transpose(pitch);
				}
			}
			else {
				freq = note.freq;
				double pitch = apply_modulation(PITCH_SCALE, osc.pitch, env_val,
					lfo_mod, controls, note.velocity, aftertouch);
				if (osc.semi || pitch) {
					freq = note_to_freq(note.note + osc.semi + pitch);
				}
				//FM
				freq += fm;
				freq *= env.pitch_bend * osc.transpose;
			}

			AnalogOscilatorData data = { osc.waveform, osc.analog, osc.sync };
			AnalogOscilatorBankData bdata = { 0.1, osc.unison_amount };

			//Apply modulation
			double volume = apply_modulation(VOLUME_SCALE, osc.volume, env_val,
				lfo_val, controls, note.velocity, aftertouch);
			data.sync_mul = apply_modulation(SYNC_SCALE, osc.sync_mul, env_val,
					lfo_val, controls, note.velocity, aftertouch);
			data.pulse_width = apply_modulation(PULSE_WIDTH_SCALE, osc.pulse_width,
					env_val, lfo_val, controls, note.velocity, aftertouch);
			bdata.unison_detune = apply_modulation(UNISON_DETUNE_SCALE,
					osc.unison_detune, env_val, lfo_val, controls, note.velocity, aftertouch);

			AnalogOscilatorSignal sig = part.oscilator.signal(freq, info.time_step, data, bdata, op.audible, modulates);
			carrier += sig.carrier * volume;
			modulator += sig.modulator * volume;
		}
		//Post processing
		//Volume
		double volume = apply_modulation(VOLUME_SCALE, op.volume, env_val,
						lfo_val, controls, note.velocity, aftertouch) * op_part.amp_env.amplitude(op.env, info.time_step,
						note.pressed, env.sustain);
		//Octave amp
		if (op.amp_kb_track_upper && note.note > op.amp_kb_track_note) {
			volume *= fmax(0, fmin(2, 1.0 + ((double) note.note - op.amp_kb_track_note)/12.0 * op.amp_kb_track_upper));
		}
		else if (op.amp_kb_track_lower && note.note < op.amp_kb_track_note) {
			volume *= fmax(0, fmin(2, 1.0 - ((double) note.note - op.amp_kb_track_note)/12.0 * op.amp_kb_track_lower));
		}
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
			if (op.filter.on) {
				//Pre drive
				if (op.filter.drive) {
					carrier = atan(carrier * (1.5 + op.filter.drive_amount * 10));
				}

				FilterData filter { op.filter.type };
				filter.cutoff = scale_cutoff(apply_modulation(FILTER_CUTOFF_SCALE,
						op.filter.cutoff, env_val, lfo_val, controls,
						note.velocity, aftertouch)); //TODO optimize
				filter.resonance = apply_modulation(FILTER_RESONANCE_SCALE,
						op.filter.resonance, env_val, lfo_val, controls,
						note.velocity, aftertouch);

				if (op.filter.kb_track) {
					double cutoff = filter.cutoff;
					//KB track
					cutoff *= 1 + ((double) note.note - op.filter.kb_track_note) / 12.0 * op.filter.kb_track;
					filter.cutoff = cutoff;
				}

				carrier = op_part.filter.apply(filter, carrier, info.time_step);
			}
			carrier *= volume;
			//Pan
			double panning = apply_modulation(PANNING_SCALE, op.panning,
					env_val, lfo_val, controls, note.velocity, aftertouch);
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
		process_note(lsample, rsample, info, note, env);
	}
}

void AnalogSynth::process_sample(double& lsample, double& rsample,
		SampleInfo &info, KeyboardEnvironment &env, EngineStatus &status) {
	//Mono
	if (preset.mono) {
		if (status.pressed_notes) {
			AnalogSynthVoice& voice = this->note.note[status.latest_note_index];
			//Update portamendo
			if (voice.note != mono_voice.note) {
				note_port.set(voice.note, info.time, first_port ? 0 : preset.portamendo * abs((int) voice.note - mono_voice.note) / 50.0);

			}
			if (!preset.legato || !mono_voice.valid) {
				mono_voice.velocity = voice.velocity;
			}
			//Trigger note
			if (!mono_voice.pressed || voice.note != mono_voice.note) {
				mono_voice.valid = true;
				mono_voice.pressed = true;
				//Reset envs to attack
				for (size_t i = 0; i < preset.mod_env_count; ++i) {
					if (!preset.legato || mono_voice.parts[i].mod_env.phase >= RELEASE) {
						//Mod env
						mono_voice.parts[i].mod_env.phase = ATTACK;
					}
				}
				for (size_t i = 0; i < preset.op_count; ++i) {
					if (!preset.legato || mono_voice.parts[i].amp_env.phase >= RELEASE) {
						//Amp env
						mono_voice.parts[i].amp_env.phase = ATTACK;
						mono_voice.parts[i].amp_env.time = 0;
						mono_voice.parts[i].amp_env.last = 0;
					}
				}
			}

			mono_voice.note = voice.note;
			mono_voice.freq = voice.freq;
			first_port = false;
		}
		else {
			mono_voice.pressed = false;
		}

		//Playback
		if (mono_voice.valid) {
			double pitch = note_port.get(info.time);
			KeyboardEnvironment e = env;
			e.pitch_bend *= note_to_freq_transpose(
					pitch - mono_voice.note);

			process_note(lsample, rsample, info, mono_voice, e);
			if (amp_finished(info, mono_voice, e)) {
				mono_voice.valid = false;
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
				lfo_val, controls, 0, aftertouch);
		AnalogOscilatorData d = { lfo.waveform };
		lfos[i].process(lfo.freq, info.time_step, d);
		lfo_val[i] = lfos[i].carrier(lfo.freq, info.time_step, d) * volume;
		lfo_mod[i] = lfos[i].modulator(lfo.freq, info.time_step, d) * volume;
	}
}

bool AnalogSynth::midi_message(MidiMessage& msg, int transpose, SampleInfo& info) {
	if (msg.type == MessageType::MONOPHONIC_AFTERTOUCH) {
		aftertouch = msg.monophonic_aftertouch()/127.0;
	}
	return BaseSoundEngine::midi_message(msg, transpose, info);
}

bool AnalogSynth::control_change(unsigned int control, unsigned int value) {
	controls[control] = value / 127.0;
	return false;
}

bool AnalogSynth::note_finished(SampleInfo &info, AnalogSynthVoice &note,
		KeyboardEnvironment &env, size_t note_index) {
	//Mono notes
	if (preset.mono) {
		return !note.pressed;
	}
	return !note.pressed && amp_finished(info, note, env);
}

void AnalogSynth::press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity) {
	AnalogSynthVoice& voice = this->note.note[this->note.press_note(info, real_note, note, velocity)];
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
				voice.parts[j].oscilator.reset(osc.phase);
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
		KeyboardEnvironment &env) {
	bool finished = true;
	for (size_t i = 0; i < preset.op_count && finished; ++i) {
		finished = note.parts[i].amp_env.is_finished();
	}
	return finished;
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
	tree.put("aftertouch_amount", mod.aftertouch_amount);
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
	mod.aftertouch_amount = tree.get<double>("aftertouch_amount", 0);
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
	data.peak_volume = tree.get("peak_volume", 1.0);
	data.sustain_time = tree.get("sustain_time", 0);
	data.release_volume = tree.get("release_volume", 0.0);

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
	tree.put("peak_volume", data.peak_volume);
	tree.put("sustain_time", data.sustain_time);
	tree.put("release_volume", data.release_volume);
	return tree;
}

static FilterEntity load_filter(boost::property_tree::ptree tree) {
	FilterEntity filter;

	filter.on = tree.get<bool>("on", false);
	filter.drive = tree.get<bool>("drive", false);
	filter.drive_amount = tree.get<double>("drive_amount", 0);
	filter.type = (FilterType) tree.get<int>("type", 0);
	filter.cutoff = load_prop_mod(tree, "cutoff");
	filter.resonance = load_prop_mod(tree, "resonance");
	filter.kb_track = tree.get<double>("kb_track", 0);
	filter.kb_track_note = tree.get<int>("kb_track_note", 36);

	return filter;
}

static FilterEntity load_filter(boost::property_tree::ptree parent, std::string path) {
	const auto& val = parent.get_child_optional(path);
	if (val) {
		return load_filter(val.get());
	}
	return {};
}

static boost::property_tree::ptree save_filter(FilterEntity filter) {
	boost::property_tree::ptree o;
	o.put("on", filter.on);
	o.put("drive", filter.drive);
	o.put("drive_amount", filter.drive_amount);
	o.put("type", (int) filter.type);
	o.add_child("cutoff", save_prop_mod(filter.cutoff));
	o.add_child("resonance", save_prop_mod(filter.resonance));
	o.put("kb_track", filter.kb_track);
	o.put("kb_track_note", filter.kb_track_note);
	return o;
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
			osc.phase = o.second.get<double>("phase", 0);
			osc.fixed_freq = o.second.get<bool>("fixed_freq", false);
			osc.note = o.second.get<unsigned int>("note", 60);

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
			op.amp_kb_track_upper = o.second.get<double>("amp_kb_track_upper", 0);
			op.amp_kb_track_lower = o.second.get<double>("amp_kb_track_lower", 0);
			op.amp_kb_track_note = o.second.get<int>("amp_kb_track_note", 60);

			op.filter.on = o.second.get<bool>("filter", false);
			op.filter.drive = o.second.get<bool>("pre_filter_drive", false);
			op.filter.drive_amount = o.second.get<double>("pre_filter_drive_amount", 0);
			op.filter.type = (FilterType) o.second.get<int>("filter_type", 0);
			op.filter.cutoff = load_prop_mod(o.second, "filter_cutoff");
			op.filter.resonance = load_prop_mod(o.second, "filter_resonance");
			op.filter.kb_track = o.second.get<double>("filter_kb_track", 0);
			op.filter.kb_track_note = o.second.get<int>("filter_kb_track_note", 36);

			op.oscilator_count = o.second.get<size_t>("oscilator_count", 1);


			const auto& fm = o.second.get_child_optional("fm");
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
			o.put("phase", osc.phase);
			o.put("fixed_freq", osc.fixed_freq);
			o.put("note", osc.note);


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
		o.put("amp_kb_track_upper", op.amp_kb_track_upper);
		o.put("amp_kb_track_lower", op.amp_kb_track_lower);
		o.put("amp_kb_track_note", op.amp_kb_track_note);

		o.put("filter", op.filter.on);
		o.put("pre_filter_drive", op.filter.drive);
		o.put("pre_filter_drive_amount", op.filter.drive_amount);
		o.put("filter_type", (int) op.filter.type);
		o.add_child("filter_cutoff", save_prop_mod(op.filter.cutoff));
		o.add_child("filter_resonance", save_prop_mod(op.filter.resonance));
		o.put("filter_kb_track", op.filter.kb_track);
		o.put("filter_kb_track_note", op.filter.kb_track_note);

		o.put("oscilator_count", op.oscilator_count);

		//FM
		for (size_t k = 0; k < ANALOG_PART_COUNT; ++k) {
			o.add("fm.amount", op.fm[k]);
		}

		tree.add_child("operators.operator", o);
	}

	return tree;
}

AnalogSynth::~AnalogSynth() {

}

template<>
std::string get_engine_name<AnalogSynth>() {
	return "Analog Synth";
}

void __fix_link_asynth_name__ () {
	get_engine_name<AnalogSynth>();
}

