/*
 * soundengine.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "soundengine.h"

#include <algorithm>

void InsertEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (effect) {
		effect->apply(lsample, rsample, info);
	}
}

Effect* InsertEffect::get_effect() const {
	return effect;
}

void InsertEffect::set_effect(Effect *effect) {
	delete this->effect;
	this->effect = effect;
}

void InsertEffect::set_effect_index(ssize_t index) {
	auto builders = device->get_effect_builders();
	if (index >= 0 && (size_t) index < builders.size()) {
		set_effect(builders.at(index)->build());
	}
	else {
		set_effect(nullptr);
	}
}

ssize_t InsertEffect::get_effect_index() {
	auto builders = device->get_effect_builders();
	ssize_t index = -1;
	for (size_t i = 0; i < builders.size(); ++i) {
		if (builders[i]->matches(effect)) {
			index = i;
			break;
		}
	}

	return index;
}

InsertEffect::~InsertEffect() {
}


void MasterEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	lsample = this->lsample;
	rsample = this->rsample;

	if (effect) {
		effect->apply(lsample, rsample, info);
	}

	this->lsample = 0;
	this->rsample = 0;
}

Effect* MasterEffect::get_effect() const {
	return effect;
}

void MasterEffect::set_effect(Effect *effect) {
	delete this->effect;
	this->effect = effect;
}

void MasterEffect::set_effect_index(ssize_t index) {
	auto builders = device->get_effect_builders();
		if (index >= 0 && (size_t) index < builders.size()) {
			set_effect(builders.at(index)->build());
		}
		else {
			set_effect(nullptr);
		}
}

ssize_t MasterEffect::get_effect_index() {
	auto builders = device->get_effect_builders();
	ssize_t index = -1;
	for (size_t i = 0; i < builders.size(); ++i) {
		if (builders[i]->matches(effect)) {
			index = i;
			break;
		}
	}

	return index;
}

MasterEffect::~MasterEffect() {
}



//SoundEngineChannel
SoundEngineChannel::SoundEngineChannel() {
	engine = nullptr;
}

void SoundEngineChannel::init_device(SoundEngineDevice* device) {
	if (!this->device) {
		this->device = device;
		for (size_t i = 0; i < effects.size(); ++i) {
			effects[i].device = device;
		}
	}
}

void SoundEngineChannel::process_sample(double& lsample, double& rsample, SampleInfo &info, Metronome& metronome, size_t scene) {
	//Properties
	if (engine) {
		SoundEngineScene& s = scenes[scene];

		if (s.active || status.pressed_notes) {
			//Arpeggiator
			if (arp.on) {
				arp.apply(info, device->metronome,
				[this, scene](SampleInfo& i, unsigned int note, double velocity) {
					engine->press_note(i, note, note + scenes[scene].source.octave * 12, velocity);
				},
				[this](SampleInfo& i, unsigned int note) {
					engine->release_note(i, note);
				});
			}
			//Process
			status = engine->process_sample(lsample, rsample, info, *device);
			//Effects
			for (size_t i = 0; i < CHANNEL_INSERT_EFFECT_AMOUNT; ++i) {
				effects[i].apply(lsample, rsample, info);
			}
			//Pan
			lsample *= (1 - fmax(0, panning));
			rsample *= (1 - fmax(0, -panning));
		}
		//Playback
		lsample *= volume;
		rsample *= volume;
	}
}

bool SoundEngineChannel::send(MidiMessage &message, SampleInfo& info, size_t scene) {
	bool updated = false;
	if (scenes[scene].active || (status.pressed_notes && message.type != MessageType::NOTE_ON)) {
		for (auto& e : effects) {
			if (e.get_effect()) {
				if (e.get_effect()->midi_message(message, info)) {
					updated = true;
				}
			}
		}
		if (arp.on) {
			switch (message.type) {
			case MessageType::NOTE_ON:
				arp.press_note(info, message.note(), message.velocity()/127.0);
				break;
			case MessageType::NOTE_OFF:
				arp.release_note(info, message.note());
				break;
			default:
				updated = engine->midi_message(message, scenes[scene].source.octave * 12, info) || updated; //FIXME
				break;
			}
		}
		else {
			updated = engine->midi_message(message, scenes[scene].source.octave * 12, info) || updated;
		}
	}
	return updated;
}

void SoundEngineChannel::set_engine_index(ssize_t engine_index) {
	auto builders = device->get_engine_builders();
	if (engine_index >= 0 && (size_t) engine_index < builders.size()) {
		set_engine(builders.at(engine_index)->build());
	}
	else {
		set_engine(nullptr);
	}
}

ssize_t SoundEngineChannel::get_engine_index() {
	auto builders = device->get_engine_builders();
	ssize_t index = -1;
	for (size_t i = 0; i < builders.size(); ++i) {
		if (builders[i]->matches(engine)) {
			index = i;
			break;
		}
	}

	return index;
}

SoundEngine* SoundEngineChannel::get_engine() {
	return engine;
}

void SoundEngineChannel::set_engine(SoundEngine* engine) {
	delete this->engine;
	this->engine = engine;
}

SoundEngineChannel::~SoundEngineChannel() {
	delete engine;
	engine = nullptr;
}

unsigned int SoundEngineChannel::get_source_channel() const {
	return scenes[device->scene].source.channel;
}

void SoundEngineChannel::set_source_channel(unsigned int channel) {
	scenes[device->scene].source.channel = channel;
}

unsigned int SoundEngineChannel::get_end_note() const {
	return scenes[device->scene].source.end_note;
}

void SoundEngineChannel::set_end_note(unsigned int endNote) {
	scenes[device->scene].source.end_note = endNote;
}

unsigned int SoundEngineChannel::get_end_velocity() const {
	return scenes[device->scene].source.end_velocity;
}

void SoundEngineChannel::set_end_velocity(unsigned int endVelocity) {
	scenes[device->scene].source.end_velocity = endVelocity;
}

ssize_t SoundEngineChannel::get_input() const {
	return scenes[device->scene].source.input;
}

void SoundEngineChannel::set_input(ssize_t input) {
	scenes[device->scene].source.input = input;
}

int SoundEngineChannel::get_octave() const {
	return scenes[device->scene].source.octave;
}

void SoundEngineChannel::set_octave(int octave) {
	scenes[device->scene].source.octave = octave;
}

unsigned int SoundEngineChannel::get_start_note() const {
	return scenes[device->scene].source.start_note;
}

void SoundEngineChannel::set_start_note(unsigned int startNote) {
	scenes[device->scene].source.start_note = startNote;
}

unsigned int SoundEngineChannel::get_start_velocity() const {
	return scenes[device->scene].source.start_velocity;
}

void SoundEngineChannel::set_start_velocity(unsigned int startVelocity) {
	scenes[device->scene].source.start_velocity = startVelocity;
}

bool SoundEngineChannel::is_transfer_cc() const {
	return scenes[device->scene].source.transfer_cc;
}

void SoundEngineChannel::set_transfer_cc(bool transferCc) {
	scenes[device->scene].source.transfer_cc = transferCc;
}

bool SoundEngineChannel::is_transfer_channel_aftertouch() const {
	return scenes[device->scene].source.transfer_channel_aftertouch;
}

void SoundEngineChannel::set_transfer_channel_aftertouch(
		bool transferChannelAftertouch) {
	scenes[device->scene].source.transfer_channel_aftertouch = transferChannelAftertouch;
}

bool SoundEngineChannel::is_transfer_other() const {
	return scenes[device->scene].source.transfer_other;
}

void SoundEngineChannel::set_transfer_other(bool transferOther) {
	scenes[device->scene].source.transfer_other = transferOther;
}

bool SoundEngineChannel::is_transfer_pitch_bend() const {
	return scenes[device->scene].source.transfer_pitch_bend;
}

void SoundEngineChannel::set_transfer_pitch_bend(bool transferPitchBend) {
	scenes[device->scene].source.transfer_pitch_bend = transferPitchBend;
}

bool SoundEngineChannel::is_transfer_prog_change() const {
	return scenes[device->scene].source.transfer_prog_change;
}

void SoundEngineChannel::set_transfer_prog_change(bool transferProgChange) {
	scenes[device->scene].source.transfer_prog_change = transferProgChange;
}

bool SoundEngineChannel::is_active() const {
	return scenes[device->scene].active;
}

void SoundEngineChannel::set_active(bool active) {
	scenes[device->scene].active = active;
}

//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice() : metronome(120){
	metronome.init(0);
	for (size_t i = 0; i < this->effects.size(); ++i) {
		effects[i].device = this;
	}
	for (size_t i = 0; i < this->channels.size(); ++i) {
		SoundEngineChannel& ch = this->channels[i];
		ch.init_device(this);
	}
}

void SoundEngineDevice::process_sample(double& lsample, double& rsample, SampleInfo &info) {
	//Motion Sequencer
	size_t motion_sequencer_amount = std::min(this->motion_sequencer_amount, (size_t) MOTION_SEQUENCER_AMOUNT);
	for (size_t i = 0; i < motion_sequencer_amount; ++i) {
		motion_sequencer_values[i] = motion_sequencers[i].amplitude(motion_sequencer_presets[i], metronome, info);
	}

	//Channels
	size_t scene = this->scene;
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		double l = 0;
		double r = 0;
		SoundEngineChannel& ch = this->channels[i];
		ch.process_sample(l, r, info, metronome, scene);

		if (ch.master_send >= 0 && ch.master_send < SOUND_ENGINE_MASTER_EFFECT_AMOUNT) {
			effects[ch.master_send].lsample += l;
			effects[ch.master_send].rsample += r;
		}
		else {
			lsample += l;
			rsample += r;
		}
	}
	//Effects
	for (size_t i = 0; i < SOUND_ENGINE_MASTER_EFFECT_AMOUNT; ++i) {
		MasterEffect& effect = effects[i];
		double l = 0;
		double r = 0;
		effect.apply(l, r, info);

		if (effect.next_effect > i && effect.next_effect < SOUND_ENGINE_MASTER_EFFECT_AMOUNT) {
			effects[effect.next_effect].lsample += l;
			effects[effect.next_effect].rsample += r;
		}
		else {
			lsample += l;
			rsample += r;
		}
	}
	//Looper
	looper.apply(lsample, rsample, metronome, info);
	//Metronome
	if (play_metronome) {
		if (metronome.is_beat(info.sample_time, info.sample_rate, 1)) {
			metronome_env.reset();
		}
		double vol = metronome_env.amplitude(metronome_env_data, info.time_step, true, false);
		if (vol) {
			double sample = sine_wave(fmod(info.time * 3520, 1)) * vol;
			lsample += sample;
			rsample += sample;
		}
	}
	lsample *= volume;
	rsample *= volume;
}

std::vector<SoundEngineBuilder*> SoundEngineDevice::get_engine_builders() {
	return engine_builders;
}

void SoundEngineDevice::add_sound_engine(SoundEngineBuilder* engine) {
	engine_builders.push_back(engine);
}

void SoundEngineDevice::add_effect(EffectBuilder* effect) {
	effect_builders.push_back(effect);
}

inline bool SoundEngineDevice::send(MidiMessage &message, size_t input, SampleInfo& info) {
	size_t scene = this->scene;
	bool updated = false;
	bool pass = true;

	switch (message.type) {
	case MessageType::POLYPHONIC_AFTERTOUCH:
	case MessageType::MONOPHONIC_AFTERTOUCH:
		//TODO set global aftertouch
	case MessageType::NOTE_ON:
	case MessageType::NOTE_OFF:
		//Notes
		for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
			ChannelSource& s = channels[i].scenes[scene].source;
			if (s.input >= 0) {
				MidiSource& source = sources[s.input];
				if (source.device == static_cast<ssize_t>(input) && source.channel == message.channel) {
					bool pass = true;

					switch (message.type) {
					case MessageType::NOTE_ON:
						pass = s.start_velocity <= message.velocity() && s.end_velocity >= message.velocity();
						/* no break */
					case MessageType::POLYPHONIC_AFTERTOUCH:
						pass = pass && s.start_note <= message.note() && s.end_note >= message.note();
						/* no break */
					case MessageType::NOTE_OFF:
						break;
					case MessageType::MONOPHONIC_AFTERTOUCH:
						pass = s.transfer_channel_aftertouch;
						break;
					default:
						break;
					}
					//Apply binding
					if (pass) {
						//Send
						SoundEngineChannel& ch = this->channels[i];
						SoundEngine* engine = ch.get_engine();
						if (engine) {
							updated = ch.send(message, info, scene) || updated;
						}
					}
				}
			}
		}
		break;
	case MessageType::CONTROL_CHANGE:
	case MessageType::PROGRAM_CHANGE:
	case MessageType::PITCH_BEND:
	case MessageType::SYSEX:
		for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
			MidiSource& source = sources[i];
			if (source.device == static_cast<ssize_t>(i)) {
				if (message.type == MessageType::SYSEX) {
					if (message.channel == 8) {
						double delta = info.time - first_beat_time;
						unsigned int old_bpm = metronome.get_bpm();
						if (delta) {
							if (clock_beat_count && clock_beat_count % 96 == 0) {
								unsigned int bpm = round(clock_beat_count/24.0 * 60.0/delta);
								metronome.set_bpm(bpm);
								if (bpm != old_bpm) {
									updated = true;
								}
								metronome.init(first_beat_time);
							}
						}
						clock_beat_count++;
					}
					else if (message.channel == 0x0A) {
						first_beat_time = info.time;
						clock_beat_count = 0;
						metronome.init(info.time);
					}
					else if (message.channel == 0x0B) {
						metronome.init(info.time);
					}
				}
				else if (source.channel < 0 || source.channel == message.channel) {
					switch (message.type) {
					case MessageType::CONTROL_CHANGE:
						//TODO global cc an pitch bend
						/* no break */
					case MessageType::PROGRAM_CHANGE:
						/* no break */
					case MessageType::PITCH_BEND:
						//Channels
						for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
							ChannelSource& s = channels[i].scenes[scene].source;
							if (s.input >= 0) {
								MidiSource& source = sources[s.input];
								if (source.device == static_cast<ssize_t>(input)) {
									bool pass = true;
									switch (message.type) {
									case MessageType::CONTROL_CHANGE:
										pass = s.transfer_cc;
										break;
									case MessageType::PROGRAM_CHANGE:
										pass = s.transfer_prog_change;
										break;
									case MessageType::PITCH_BEND:
										pass = s.transfer_pitch_bend;
										break;
									default:
										break;
									}
									//Apply binding
									if (pass) {
										//Send
										SoundEngineChannel& ch = this->channels[i];
										SoundEngine* engine = ch.get_engine();
										if (engine) {
											updated = ch.send(message, info, scene) || updated;
										}
									}
								}
							}
						}
						//Effects
						for (auto& e : effects) {
							if (e.get_effect()) {
								if (e.get_effect()->midi_message(message, info)) {
									updated = true;
								}
							}
						}
						break;
					default:
						break;
					}
				}
			}
		}
		break;
	case MessageType::INVALID:
		break;
	}

	return updated;
}


void SoundEngineDevice::apply_program(Program* program) {
	//Global
	metronome.set_bpm(program->metronome_bpm);
	scene = 0;
	//Channels
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		ChannelProgram& prog = program->channels[i];
		SoundEngineChannel& ch = channels[i];

		ch.set_engine_index(prog.engine_index);
		ch.volume = prog.volume;
		ch.panning = prog.panning;
		ch.scenes = prog.scenes;
		ch.arp.on = prog.arp_on;
		ch.arp.metronome.set_bpm(prog.arpeggiator_bpm);
		ch.arp.preset = prog.arpeggiator;

		//Engine
		SoundEngine* engine = ch.get_engine();
		if (engine) {
			engine->apply_program(prog.engine_program);
		}
		ch.master_send = prog.send_master;
		//Effects
		for (size_t i = 0; i < CHANNEL_INSERT_EFFECT_AMOUNT; ++i) {
			InsertEffectProgram& p = prog.effects[i];
			InsertEffect& effect = ch.effects[i];

			effect.set_effect_index(p.effect);
			if (effect.get_effect()) {
				effect.get_effect()->apply_program(p.prog);
			}
		}
	}
	//Master Effects
	for (size_t i = 0; i < SOUND_ENGINE_MASTER_EFFECT_AMOUNT; ++i) {
		MasterEffectProgram& prog = program->effects[i];
		MasterEffect& effect = effects[i];

		effect.next_effect = prog.next_effect;
		effect.set_effect_index(prog.effect);
		if (effect.get_effect()) {
			effect.get_effect()->apply_program(prog.prog);
		}
	}
}

void SoundEngineDevice::save_program(Program* program) {
	//Global
	program->metronome_bpm = metronome.get_bpm();
	//Channels
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		ChannelProgram& prog = program->channels[i];
		SoundEngineChannel& ch = channels[i];
		prog.engine_index = ch.get_engine_index();
		prog.volume = ch.volume;
		prog.panning = ch.panning;
		prog.scenes = ch.scenes;
		prog.arp_on = ch.arp.on;
		prog.arpeggiator_bpm = ch.arp.metronome.get_bpm();
		prog.arpeggiator = ch.arp.preset;

		//Engine
		SoundEngine* engine = ch.get_engine();
		if (engine) {
			engine->save_program(&prog.engine_program);
		}
		prog.send_master = ch.master_send;
		//Effects
		for (size_t i = 0; i < CHANNEL_INSERT_EFFECT_AMOUNT; ++i) {
			InsertEffectProgram& p = prog.effects[i];
			InsertEffect& effect = ch.effects[i];

			p.effect = effect.get_effect_index();
			if (effect.get_effect()) {
				effect.get_effect()->save_program(&p.prog);
			}
		}
	}
	//Master Effects
	for (size_t i = 0; i < SOUND_ENGINE_MASTER_EFFECT_AMOUNT; ++i) {
		MasterEffectProgram& prog = program->effects[i];
		MasterEffect& effect = effects[i];

		prog.next_effect = effect.next_effect;
		prog.effect = effect.get_effect_index();
		if (effect.get_effect()) {
			effect.get_effect()->save_program(&prog.prog);
		}
	}
}

bool SoundEngineDevice::send_engine(MidiMessage &message, SampleInfo &info) {
	bool updated = false;
	//Time change
	if (message.type == MessageType::SYSEX) {

	}
	//Scene change
	if (message.type == MessageType::CONTROL_CHANGE) {
		for (size_t i = 0; i < SOUND_ENGINE_SCENE_AMOUNT; ++i) {
			if (scene_ccs[i] == message.control()) {
				scene = i;
				updated = true;
			}
		}
	}
	return updated;
}

SoundEngineDevice::~SoundEngineDevice() {
	//Clear channels
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i].set_engine_index(-1);
	}
	//Delete engines
	for (SoundEngineBuilder* engine : engine_builders) {
		delete engine;
	}
	engine_builders.clear();
	//Delete effects
	for (EffectBuilder* effect : effect_builders) {
		delete effect;
	}
	effect_builders.clear();
}
