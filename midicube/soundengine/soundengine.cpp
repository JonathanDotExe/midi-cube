/*
 * soundengine.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "soundengine.h"

#include <algorithm>

//SoundEngineChannel
SoundEngineChannel::SoundEngineChannel() {
	engine = nullptr;
}

void SoundEngineChannel::process_sample(double& lsample, double& rsample, SampleInfo &info, Metronome& metronome, size_t scene) {
	//Properties
	if (engine) {
		double l = 0;
		double r = 0;

		SoundEngineScene& s = scenes[scene];

		if (s.active || status.pressed_notes) {
			//Arpeggiator
			if (arp.on) {
				arp.apply(info,
				[this](SampleInfo& i, unsigned int note, double velocity) {
					engine->press_note(i, note, velocity);
				},
				[this](SampleInfo& i, unsigned int note) {
					engine->release_note(i, note);
				});
			}
			//Process
			status = engine->process_sample(l, r, info);
			//Vocoder
			vocoder.apply(l, r, info.input_sample, vocoder_preset, info);
			//Bit Crusher
			bitcrusher.apply(l, r, bitcrusher_preset, info);
			//Pan
			l *= (1 - fmax(0, panning));
			r *= (1 - fmax(0, -panning));
		}
		//Looper
		looper.apply(l, r, metronome, info);
		//Playback
		lsample += l * volume;
		rsample += r * volume;
	}
}

bool SoundEngineChannel::send(MidiMessage &message, SampleInfo& info, size_t scene) {
	if (scenes[scene].active || (status.pressed_notes && message.type != MessageType::NOTE_ON)) {
		if (arp.on) {
			switch (message.type) {
			case MessageType::NOTE_ON:
				arp.note.press_note(info, message.note(), message.velocity()/127.0);
				break;
			case MessageType::NOTE_OFF:
				arp.note.release_note(info, message.note(), true);
				break;
			default:
				return engine->midi_message(message, info); //FIXME
				break;
			}
		}
		else {
			return engine->midi_message(message, info);
		}
	}
	return false;
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
	for (size_t i = 0; i < this->channels.size(); ++i) {
		SoundEngineChannel& ch = this->channels[i];
		ch.init_device(this);
	}
}

void SoundEngineDevice::process_sample(double& lsample, double& rsample, SampleInfo &info) {
	//Channels
	size_t scene = this->scene;
	for (size_t i = 0; i < this->channels.size(); ++i) {
		SoundEngineChannel& ch = this->channels[i];
		ch.process_sample(lsample, rsample, info, metronome, scene);
	}
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

bool SoundEngineDevice::send(MidiMessage &message, SampleInfo& info) {
	SoundEngineChannel& ch = this->channels[message.channel];
	SoundEngine* engine = ch.get_engine();
	if (engine) {
		return ch.send(message, info, scene);
	}
	return false;
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
	}
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
}
