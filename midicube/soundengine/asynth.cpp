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

AnalogSynth::AnalogSynth() {
	//Parts
	for (size_t i = 0; i < ANALOG_PART_COUNT; ++i) {
		LFOEntity& lfo = preset.lfos[i];
		binder.add_binding(&lfo.volume.value);
		binder.add_binding(&lfo.freq);

		ModEnvelopeEntity& mod_env = preset.mod_envs[i];
		binder.add_binding(&mod_env.volume.value);
		mod_env.env.add_bindings(binder);

		OscilatorEntity& osc = preset.oscilators[i];
		binder.add_binding(&osc.volume.value);
		binder.add_binding(&osc.sync_mul.value);
		binder.add_binding(&osc.pulse_width.value);
		binder.add_binding(&osc.unison_detune.value);
		binder.add_binding(&osc.pitch.value);

		OperatorEntity& op = preset.operators[i];
		op.env.add_bindings(binder);
		binder.add_binding(&op.volume.value);
		binder.add_binding(&op.panning.value);

		binder.add_binding(&op.first_filter.cutoff.value);
		binder.add_binding(&op.first_filter.resonance.value);
		binder.add_binding(&op.first_filter.kb_track);

		binder.add_binding(&op.second_filter.cutoff.value);
		binder.add_binding(&op.second_filter.resonance.value);
		binder.add_binding(&op.second_filter.kb_track);
	}
	//Global
	binder.add_binding(&preset.portamendo);
}

inline double AnalogSynth::apply_modulation(const FixedScale &scale, PropertyModulation &mod, double velocity, double aftertouch, std::array<double, ANALOG_PART_COUNT>& lfo_val) {
	double prog = mod.value;
	prog += env_val[mod.mod_env] * mod.mod_env_amount
			+ lfo_val[mod.lfo] * mod.lfo_amount * lfo_vol[mod.lfo]
			+ velocity * mod.velocity_amount + aftertouch * mod.aftertouch_amount;
	prog = fmin(fmax(prog, 0.0), 1.0);
	return scale.value(prog);
}

void AnalogSynth::apply_filter(FilterEntity filter, Filter& f, double& carrier, AnalogSynthVoice &note, double time_step, double velocity, double aftertouch) {
	//Filter
	if (filter.on) {
		//Pre drive
		if (filter.drive) {
			carrier = atan(carrier * (1.5 + filter.drive_amount * 10));
		}

		FilterData data { filter.type };
		data.cutoff = scale_cutoff(
				apply_modulation(FILTER_CUTOFF_SCALE, filter.cutoff, velocity, aftertouch, lfo_val)); //TODO optimize
		data.resonance = apply_modulation(FILTER_RESONANCE_SCALE, filter.resonance, velocity,
				aftertouch, lfo_val);

		if (filter.kb_track) {
			double cutoff = data.cutoff;
			//KB track
			cutoff *= 1
					+ ((double) note.note - filter.kb_track_note) / 12.0
							* filter.kb_track;
			data.cutoff = cutoff;
		}

		carrier = f.apply(data, carrier, time_step);
	}
}

void AnalogSynth::process_note(double& lsample, double& rsample,
		SampleInfo &info, AnalogSynthVoice &note, KeyboardEnvironment &env) {
	//Aftertouch
	double aftertouch = this->aftertouch.get(info.time);
	double velocity = note.velocity;

	if (aftertouch) {
		if (aftertouch > note.max_aftertouch) {
			note.max_aftertouch = aftertouch;
		}
	}
	if (preset.max_aftertouch) {
		aftertouch = note.max_aftertouch;
	}
	if (preset.velocity_aftertouch_amount) {
		velocity = fmin(1, note.velocity + note.max_aftertouch * preset.velocity_aftertouch_amount);
	}

	//Mod Envs
	for (size_t i = 0; i < preset.mod_env_count; ++i) {
		ModEnvelopeEntity &mod_env = preset.mod_envs[i];
		ADSREnvelopeData data = mod_env.env;
		double volume = apply_modulation(VOLUME_SCALE, mod_env.volume, velocity, aftertouch, lfo_val);
		env_val[i] = note.parts[i].mod_env.amplitude(data, info.time_step, note.pressed, env.sustain)* volume;
	}
	//LFOs
	for (size_t i = 0; i < preset.lfo_count; ++i) {
		lfo_vol[i] = apply_modulation(VOLUME_SCALE, preset.lfos[i].volume, velocity, aftertouch, lfo_val);
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
				double pitch = apply_modulation(PITCH_SCALE, osc.pitch, velocity, aftertouch, lfo_mod);
				if (pitch) {
					freq *= note_to_freq_transpose(pitch);
				}
			}
			else {
				freq = note.freq;
				double pitch = apply_modulation(PITCH_SCALE, osc.pitch, velocity, aftertouch, lfo_mod);
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
			double volume = apply_modulation(VOLUME_SCALE, osc.volume, velocity, aftertouch, lfo_val);
			data.sync_mul = apply_modulation(SYNC_SCALE, osc.sync_mul, velocity, aftertouch, lfo_val);
			data.pulse_width = apply_modulation(PULSE_WIDTH_SCALE, osc.pulse_width, velocity, aftertouch, lfo_val);
			bdata.unison_detune = apply_modulation(UNISON_DETUNE_SCALE, osc.unison_detune, velocity, aftertouch, lfo_val);

			AnalogOscilatorSignal sig = part.oscilator.signal(freq, info.time_step, data, bdata, op.audible, modulates);
			carrier += sig.carrier * volume;
			modulator += sig.modulator * volume;
		}
		//Post processing
		//Volume
		ADSREnvelopeData data = op.env;
		double volume = apply_modulation(VOLUME_SCALE, op.volume, velocity, aftertouch, lfo_val) * op_part.amp_env.amplitude(data, info.time_step,
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
			if (op.filter_parallel) {
				double c = carrier;
				apply_filter(op.first_filter, op_part.filter1, carrier, note, info.time_step, velocity, aftertouch);
				apply_filter(op.second_filter, op_part.filter2, c, note, info.time_step, velocity, aftertouch);
				carrier += c;
			}
			else {
				apply_filter(op.first_filter, op_part.filter1, carrier, note, info.time_step, velocity, aftertouch);
				apply_filter(op.second_filter, op_part.filter2, carrier, note, info.time_step, velocity, aftertouch);
			}
			carrier *= volume;
			//Pan
			double panning = apply_modulation(PANNING_SCALE, op.panning, velocity, aftertouch, lfo_val);
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
				note_port.set(voice.note, info.time, first_port ? 0 : preset.portamendo * std::abs((double) ((int) voice.note) - mono_voice.note) / 50.0);

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
						mono_voice.parts[i].mod_env.phase = HOLD;
						mono_voice.parts[i].amp_env.time = 0;
						mono_voice.parts[i].amp_env.last = 0;
					}
				}
				for (size_t i = 0; i < preset.op_count; ++i) {
					if (!preset.legato || mono_voice.parts[i].amp_env.phase >= RELEASE) {
						//Amp env
						mono_voice.parts[i].amp_env.phase = HOLD;
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
		if (lfo.motion_sequencer >= 0 && lfo.motion_sequencer < MOTION_SEQUENCER_AMOUNT) {
			lfo_val[i] = device->motion_sequencer_values[lfo.motion_sequencer];
			lfo_mod[i] = 0; //TODO
		}
		else {
			AnalogOscilatorData d = { lfo.waveform };
			double freq = lfo.freq;
			//Sync
			if (lfo.sync_master) {
				double value = lfo.clock_value;
				if (lfo.clock_value <= 0) {
					value = 1.0/(-fmin(lfo.clock_value, -1));
				}
				freq = device->metronome.get_bpm()/60.0 * value;
				if (device->metronome.is_beat(info.sample_time, info.sample_rate, value)) {
					lfos[i].reset(lfo.sync_phase);
				}
			}
			lfos[i].process(freq, info.time_step, d);
			lfo_val[i] = lfos[i].carrier(lfo.freq, info.time_step, d);
			lfo_mod[i] = lfos[i].modulator(lfo.freq, info.time_step, d);
		}
	}
}

bool AnalogSynth::midi_message(MidiMessage& msg, int transpose, SampleInfo& info, KeyboardEnvironment& env, size_t polyphony_limit) {
	if (msg.type == MessageType::MONOPHONIC_AFTERTOUCH) {
		double at = msg.monophonic_aftertouch()/127.0;
		if (at > aftertouch.get(info.time)) {
			aftertouch.set(at, info.time, preset.aftertouch_attack);
		}
		else {
			aftertouch.set(at, info.time, preset.aftertouch_release);
		}
	}
	return BaseSoundEngine::midi_message(msg, transpose, info, env, polyphony_limit);
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

void AnalogSynth::press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit) {
	AnalogSynthVoice& voice = this->note.note[this->note.press_note(info, real_note, note, velocity, polyphony_limit)];
	voice.aftertouch = 0;
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
	mod.value.save(tree, "value");
	tree.put("mod_env", mod.mod_env);
	tree.put("mod_env_amount", mod.mod_env_amount);
	tree.put("lfo", mod.lfo);
	tree.put("lfo_amount", mod.lfo_amount);
	tree.put("velocity_amount", mod.velocity_amount);
	tree.put("aftertouch_amount", mod.aftertouch_amount);
	return tree;
}


static PropertyModulation load_prop_mod(boost::property_tree::ptree tree) {
	PropertyModulation mod{0};
	mod.value.load(tree, "value", 0);
	mod.mod_env = tree.get<size_t>("mod_env", 0);
	mod.mod_env_amount = tree.get<double>("mod_env_amount", 0);
	mod.lfo = tree.get<size_t>("lfo", 0);
	mod.lfo_amount = tree.get<double>("lfo_amount", 0);
	mod.velocity_amount = tree.get<double>("velocity_amount", 0);
	mod.aftertouch_amount = tree.get<double>("aftertouch_amount", 0);
	return mod;
}

static PropertyModulation load_prop_mod(boost::property_tree::ptree parent, std::string path) {
	const auto& val = parent.get_child_optional(path);
	if (val) {
		return load_prop_mod(val.get());
	}
	return {0};
}

static BindableADSREnvelopeData load_adsr(boost::property_tree::ptree tree) {
	BindableADSREnvelopeData data;

	data.attack.load(tree, "attack", 0.0005);
	data.decay.load(tree, "decay", 0.0);
	data.sustain.load(tree, "sustain", 1.0);
	data.release.load(tree, "release", 0.0005);

	data.attack_shape = (ADSREnvelopeShape) tree.get<unsigned int>("attack_shape", 0);
	data.pre_decay_shape = (ADSREnvelopeShape) tree.get<unsigned int>("pre_decay_shape", 1);
	data.decay_shape = (ADSREnvelopeShape) tree.get<unsigned int>("decay_shape", 1);
	data.release_shape = (ADSREnvelopeShape) tree.get<unsigned int>("release_shape", 1);

	data.pre_decay.load(tree, "pre_decay", 0.0);
	data.hold.load(tree, "hold", 0.0);

	data.attack_hold.load(tree, "attack_hold", 0.0);
	data.peak_volume.load(tree, "peak_volume", 1.0);
	data.sustain_time.load(tree, "sustain_time", 0);
	data.release_volume.load(tree, "release_volume", 0.0);

	return data;
}


static BindableADSREnvelopeData load_adsr(boost::property_tree::ptree parent, std::string path) {
	const auto& val = parent.get_child_optional(path);
	if (val) {
		return load_adsr(val.get());
	}
	return {0.0005, 0, 1, 0.0005};
}


static boost::property_tree::ptree save_adsr(BindableADSREnvelopeData& data) {
	boost::property_tree::ptree tree;
	data.attack.save(tree, "attack");
	data.decay.save(tree, "decay");
	data.sustain.save(tree, "sustain");
	data.release.save(tree, "release");

	tree.put("attack_shape", (unsigned int) data.attack_shape);
	tree.put("pre_decay_shape", (unsigned int) data.pre_decay_shape);
	tree.put("decay_shape", (unsigned int) data.decay_shape);
	tree.put("release_shape", (unsigned int) data.release_shape);

	data.pre_decay.save(tree, "pre_decay");
	data.hold.save(tree, "hold");

	data.attack_hold.save(tree, "attack_hold");
	data.peak_volume.save(tree, "peak_volume");
	data.sustain_time.save(tree, "sustain_time");
	data.release_volume.save(tree, "release_volume");
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
	filter.kb_track.load(tree, "kb_track", 0);
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
	filter.kb_track.save(o, "kb_track");
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
	preset.portamendo.load(tree, "portamendo", 0.0);

	preset.aftertouch_attack = tree.get<double>("aftertouch_attack", 0.0);
	preset.aftertouch_release = tree.get<double>("aftertouch_release", 0.0);
	preset.max_aftertouch = tree.get<bool>("max_aftertouch", false);
	preset.velocity_aftertouch_amount = tree.get<double>("velocity_aftertouch_amount", 0.0);

	//LFOs
	const auto& lfos = tree.get_child_optional("lfos");
	if (lfos) {
		size_t i = 0;
		for (pt::ptree::value_type& lfo : lfos.get()) {
			if (i >= ANALOG_PART_COUNT) {
				break;
			}
			preset.lfos[i].volume = load_prop_mod(lfo.second, "volume");
			preset.lfos[i].freq.load(lfo.second, "freq", 1);
			preset.lfos[i].sync_master = lfo.second.get<bool>("sync_master", false);
			preset.lfos[i].clock_value = lfo.second.get<int>("clock_value", 1);
			preset.lfos[i].sync_phase = lfo.second.get<double>("sync_phase", 0);
			preset.lfos[i].waveform = (AnalogWaveForm) lfo.second.get<int>("waveform", 0);
			preset.lfos[i].motion_sequencer = lfo.second.get<int>("motion_sequencer", -1);
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

			op.first_filter = load_filter(o.second, "first_filter");
			op.second_filter = load_filter(o.second, "second_filter");

			op.filter_parallel = o.second.get<bool>("filter_parallel", false);
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

	tree.put("aftertouch_attack", preset.aftertouch_attack);
	tree.put("aftertouch_release", preset.aftertouch_release);
	tree.put("max_aftertouch", preset.max_aftertouch);
	tree.put("velocity_aftertouch_amount", preset.velocity_aftertouch_amount);

	//LFOs
	for (size_t i = 0; i < preset.lfo_count; ++i) {
		boost::property_tree::ptree lfo;
		lfo.add_child("volume", save_prop_mod(preset.lfos[i].volume));
		preset.lfos[i].freq.save(lfo, "freq");
		lfo.put("sync_master", preset.lfos[i].sync_master);
		lfo.put("clock_value", preset.lfos[i].clock_value);
		lfo.put("sync_phase", preset.lfos[i].sync_phase);
		lfo.put("waveform", (int) preset.lfos[i].waveform);
		lfo.put("motion_sequencer", preset.lfos[i].motion_sequencer);

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

		o.add_child("first_filter", save_filter(op.first_filter));
		o.add_child("second_filter", save_filter(op.second_filter));

		o.put("filter_parallel", op.filter_parallel);
		o.put("oscilator_count", op.oscilator_count);

		//FM
		for (size_t k = 0; k < ANALOG_PART_COUNT; ++k) {
			o.add("fm.amount", op.fm[k]);
		}

		tree.add_child("operators.operator", o);
	}

	return tree;
}

void AnalogSynth::init(SoundEngineChannel *channel) {
	SoundEngine::init(channel);
	binder.init(&channel->get_device()->binding_handler);
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

