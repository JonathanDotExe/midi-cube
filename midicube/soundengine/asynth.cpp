/*
 * asynth.cpp
 *
 *  Created on: Nov 15, 2020
 *      Author: jojo
 */
#include "asynth.h"
#include <cmath>

#define OSC_INDEX(note_index,i) (note_index + i * SOUND_ENGINE_POLYPHONY)
#define ENV_INDEX(note_index,i) (note_index + i * SOUND_ENGINE_POLYPHONY)

void apply_preset(SynthFactoryPreset type, AnalogSynthPreset& preset) {
	switch (type) {
	case SQUASHY_BASS:
	{
		ModEnvelopeEntity& mod_env = preset.mod_envs.at(0);
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
		osc.filter_resonance.value = 0.8;
	}
		break;
	case UNISON_SAWS:
	{
		OscilatorEntity& osc = preset.oscilators.at(0);
		osc.reset = true;
		osc.unison_amount = 2;
		osc.active = true;
		osc.env = {0.0005, 0, 1, 0.003};
	}
		break;
	case POLY_SWEEP:
	{
		OscilatorEntity& osc1 = preset.oscilators.at(0);
		osc1.volume.value = 0.4;
		osc1.unison_amount = 7;
		osc1.unison_detune.value = 0.05;
		osc1.active = true;
		osc1.env = {0.0005, 0, 1, 0.1};
		osc1.filter = true;
		osc1.filter_type = FilterType::LP_24;
		osc1.filter_cutoff.value = 0.1;
		osc1.filter_cutoff.cc_amount = 0.9;

		OscilatorEntity& osc2 = preset.oscilators.at(1);
		osc2.volume.value = 0.4;
		osc2.unison_amount = 3;
		osc2.unison_detune.value = 0.05;
		osc2.semi = 12;
		osc2.active = true;
		osc2.env = {0.0005, 0, 1, 0.1};
		osc2.filter = true;
		osc2.filter_type = FilterType::LP_24;
		osc2.filter_cutoff.value = 0.1;
		osc2.filter_cutoff.cc_amount = 0.9;
	}
		break;
	case SPOOKY_SINE:
	{
		preset.mono = true;
		preset.legato = true;
		preset.portamendo = 0.1;

		LFOEntity& lfo = preset.lfos.at(0);
		lfo.active = true;
		lfo.freq = 6;
		lfo.volume.value = 0;
		lfo.volume.cc_amount = 1;

		OscilatorEntity& osc = preset.oscilators.at(0);
		osc.waveform = AnalogWaveForm::SINE;
		osc.active = true;
		osc.env = {0.0005, 0, 1, 0.003};
		osc.pitch.lfo = 0;
		osc.pitch.lfo_amount = 0.125;
	}
		break;
	case LUSH_LEAD:
	{
		LFOEntity& lfo = preset.lfos.at(0);
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
		osc.panning.lfo_amount = 1;
	}
		break;
	case PULSE_BASS:
	{
		ModEnvelopeEntity& mod_env = preset.mod_envs.at(0);
		mod_env.active = true;
		mod_env.env = {0, 0.1, 0, 0.1};

		OscilatorEntity& osc = preset.oscilators.at(0);
		osc.active = true;
		osc.waveform = AnalogWaveForm::SQUARE;
		osc.pulse_width.value = 0.66;
		osc.env = {0.0005, 0.4, 0, 0.003};
		osc.filter = true;
		osc.filter_type = FilterType::LP_24;
		osc.filter_cutoff.value = 0.05;
		osc.filter_cutoff.mod_env = 0;
		osc.filter_cutoff.mod_amount = 0.2;
		osc.filter_resonance.value = 0.5;
	}
		break;
	case DELAY_CHORDS:
	{
		preset.delay_mix = 0.5;
		preset.delay_feedback = 0.3;
		preset.delay_time = 0.3;

		ModEnvelopeEntity& env = preset.mod_envs.at(0);
		env.active = true;
		env.env = {0, 0.2, 0, 0.05};

		OscilatorEntity& osc = preset.oscilators.at(0);
		osc.active = true;
		osc.unison_amount = 1;
		osc.unison_detune.value = 0.06;
		osc.env = {0.01, 0, 1, 0.1};

		osc.filter = true;
		osc.filter_type = FilterType::LP_24;
		osc.filter_kb_track = 1;
		osc.filter_cutoff.value = 0.002;
		osc.filter_cutoff.mod_amount = 0.5;
		osc.filter_resonance.value = 0.4;
	}
		break;
	case STRONG_PAD:
	{
		OscilatorEntity& osc = preset.oscilators.at(0);
		osc.waveform = AnalogWaveForm::SAW_DOWN;
		osc.active = true;
		osc.unison_amount = 3;
		osc.unison_detune.value = 0.05;
		osc.randomize = true;
		osc.env = {0.2, 0, 1, 0.4};

		osc.filter = true;
		osc.filter_cutoff.value = 0.05;	//TODO sample rate hardcoded
	}
		break;
	case CLEAN_SAW:
	{
		OscilatorEntity& osc = preset.oscilators.at(0);
		osc.waveform = AnalogWaveForm::SAW_DOWN;
		osc.active = true;
		osc.env = {0.0005, 0, 1, 0.003};
	}
		break;
	case FM_BASS:
	{
		OscilatorEntity& osc1 = preset.oscilators.at(0);
		osc1.transpose = 0.7;
		osc1.waveform = AnalogWaveForm::SINE;
		osc1.active = true;
		osc1.audible = false;
		osc1.env = {0, 0.2, 0, 0.003};

		OscilatorEntity& osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::SINE;
		osc2.active = true;
		osc2.fm.push_back({10, 0});
		osc2.env = {0.0005, 0.5, 0.0, 0.003};
	}
		break;
	case FM_E_PIANO:
	{
		OscilatorEntity& osc1 = preset.oscilators.at(0);
		osc1.waveform = AnalogWaveForm::SINE;
		osc1.active = true;
		osc1.audible = false;
		osc1.env = {0.0005, 0.6, 0.6, 0.2};

		OscilatorEntity& osc2 = preset.oscilators.at(1);
		osc2.waveform = AnalogWaveForm::SINE;
		osc2.volume.value = 0.2;
		osc2.volume.velocity_amount = 0.3;
		osc2.active = true;
		osc2.fm.push_back({0.8, 0});
		osc2.env = {0.0005, 1.8, 0.0, 0.1};

		OscilatorEntity& osc3 = preset.oscilators.at(2);
		osc3.waveform = AnalogWaveForm::SINE;
		osc3.volume.value = 0.0;
		osc3.volume.velocity_amount = 1;
		osc3.transpose = 10;
		osc3.active = true;
		osc3.audible = false;
		osc3.env = {0.0005, 0.2, 0.4, 0.2};

		OscilatorEntity& osc4 = preset.oscilators.at(3);
		osc4.waveform = AnalogWaveForm::SINE;
		osc4.volume.value = 0.5;
		osc4.active = true;
		osc4.fm.push_back({1.5, 2});
		osc4.env = {0.0005, 1.7, 0.0, 0.1};
	}
		break;
	}
}

AnalogSynth::AnalogSynth() {

}

static double apply_modulation(const FixedScale& scale, PropertyModulation& mod, std::array<double, ANALOG_MOD_ENV_COUNT>& env_val, std::array<double, ANALOG_LFO_COUNT>& lfo_val, std::array<double, ANALOG_CONTROL_COUNT>& controls, double velocity) {
	double prog = mod.value;
	prog += env_val.at(mod.mod_env) * mod.mod_amount + lfo_val.at(mod.lfo) * mod.lfo_amount + controls.at(mod.cc) * mod.cc_amount + velocity * mod.velocity_amount;
	prog = fmin(fmax(prog, 0.0), 1.0);
	return scale.value(prog);
}

void AnalogSynth::process_note(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	env_val = {};
	//Reset amps
	bool reset_amps = amp_finished(info, note, env, note_index); //TODO maybe use press note event
	//Mod Envs
	for (size_t i = 0; i < preset.mod_envs.size(); ++i) {
		ModEnvelopeEntity& mod_env = preset.mod_envs[i];
		if (mod_env.active) {
			size_t index = ENV_INDEX(note_index, i);
			if  (reset_amps) {
				mod_envs.at(index).reset();
			}

			double volume = apply_modulation(VOLUME_SCALE, mod_env.volume, env_val, lfo_val, controls, note.velocity);
			env_val.at(i) = mod_envs.at(note_index + i * SOUND_ENGINE_POLYPHONY).amplitude(mod_env.env, info.time_step, note.pressed, env.sustain) * volume;
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
			size_t fm_count = osc.fm.size();
			for (size_t i = 0; i < fm_count; ++i) {
				FrequencyModulatotion& fm = osc.fm[i];
				if (fm.fm_amount) {
					freq += modulators.at(OSC_INDEX(note_index, fm.modulator)) * fm.fm_amount;
				}
			}
			double pitch = apply_modulation(PITCH_SCALE, osc.pitch, env_val, lfo_mod, controls, note.velocity);
			if (osc.semi || pitch) {
				freq = note_to_freq(note.note + osc.semi + pitch);
			}
			freq *= env.pitch_bend * osc.transpose;

			AnalogOscilatorData data = {osc.waveform, osc.analog, osc.sync};
			AnalogOscilatorBankData bdata = {0.1, osc.unison_amount};
			size_t index = OSC_INDEX(note_index, i);
			//Only on note start
			if  (reset_amps) {
				amp_envs.at(index).reset();
				if (osc.reset) {
					oscilators.reset(index);
				}
				else if (osc.randomize) {
					oscilators.randomize(index);
				}
			}

			//Apply modulation
			double volume = apply_modulation(VOLUME_SCALE, osc.volume, env_val, lfo_val, controls, note.velocity) * amp_envs.at(index).amplitude(osc.env, info.time_step, note.pressed, env.sustain);
			data.sync_mul = apply_modulation(SYNC_SCALE, osc.sync_mul, env_val, lfo_val, controls, note.velocity);
			data.pulse_width = apply_modulation(PULSE_WIDTH_SCALE, osc.pulse_width, env_val, lfo_val, controls, note.velocity);
			bdata.unison_detune = apply_modulation(UNISON_DETUNE_SCALE, osc.unison_detune, env_val, lfo_val, controls, note.velocity);

			//Signal
			AnalogOscilatorSignal sig = oscilators.signal(freq, info.time_step, index, data, bdata);
			double signal = sig.carrier;
			modulators.at(index) = sig.modulator * volume;
			if (osc.audible) {
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
		unsigned int note = status.latest_note->note;
		//Update portamendo
		if (note != last_note) {
			//Reset envs to attack
			if (!preset.legato) {
				//Mod envs
				for (size_t i = 0; i < preset.mod_envs.size(); ++i) {
					size_t index = ENV_INDEX(0, i);	//Updating every amp might be a bug source
					mod_envs.at(index).phase = ATTACK;
				}
				//Amp envs
				for (size_t i = 0; i < preset.oscilators.size(); ++i) {
					size_t index = OSC_INDEX(0, i);
					amp_envs.at(index).phase = ATTACK;
				}
			}
			note_port.set(note, info.time, first_port ? 0 : preset.portamendo);
		}
		last_note = note;
		first_port = false;
		double pitch = note_port.get(info.time);
		KeyboardEnvironment e = env;
		e.pitch_bend *= note_to_freq_transpose(pitch - status.latest_note->note);

		process_note(channels, info, *status.latest_note, e, 0);
	}
	//Delay lines
	if (preset.delay_mix) {
		//Get samples
		double lsample = 0;
		double rsample = 0;
		for (size_t i = 0; i < channels.size(); ++i) {
			if (i%2 == 0) {
				lsample += channels[i];
			}
			else {
				rsample += channels[i];
			}
		}
		//Apply delay
		ldelay.add_isample(lsample, preset.delay_time * info.sample_rate);
		rdelay.add_isample(rsample, preset.delay_time * info.sample_rate);

		double ldsample = ldelay.process();
		double rdsample = rdelay.process();

		ldelay.add_sample(ldsample * preset.delay_feedback, preset.delay_time * info.sample_rate - 1);
		rdelay.add_sample(rdsample * preset.delay_feedback, preset.delay_time * info.sample_rate - 1);
		//Play delay
		lsample *= 1 - (fmax(0, preset.delay_mix - 0.5) * 2);
		rsample *= 1 - (fmax(0, preset.delay_mix - 0.5) * 2);

		lsample += ldsample * fmin(0.5, preset.delay_mix) * 2;
		rsample += rdsample * fmin(0.5, preset.delay_mix) * 2;

		for (size_t i = 0; i < channels.size(); ++i) {
			if (i%2 == 0) {
				channels[i] += lsample;
			}
			else {
				channels[i] += rsample;
			}
		}
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
	//Mono notes
	if (preset.mono) {	//TODO not very easy to read/side effects
		note_index = 0;
	}
	return !note.pressed && amp_finished(info, note, env, note_index);
}

bool AnalogSynth::amp_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	bool finished = true;
	for (size_t i = 0; i < preset.oscilators.size() && finished; ++i) {
		OscilatorEntity& osc = preset.oscilators[i];
		size_t index = OSC_INDEX(note_index, i);
		if (osc.active) {
			finished = amp_envs.at(index).is_finished();
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

