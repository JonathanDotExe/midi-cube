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
	engine_index = -1;
}

void SoundEngineChannel::process_sample(double& lsample, double& rsample, SampleInfo &info, Metronome& metronome, SoundEngine* engine, size_t scene) {
	//Properties
	if (engine) {
		double l = 0;
		double r = 0;

		SoundEngineScene& s = scenes[scene];

		if (s.active || status.pressed_notes) {
			//Arpeggiator
			if (arp.on) {
				arp.apply(info,
				[engine](SampleInfo& i, unsigned int note, double velocity) {
					engine->press_note(i, note, velocity);
				},
				[engine](SampleInfo& i, unsigned int note) {
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

void SoundEngineChannel::send(MidiMessage &message, SampleInfo& info, SoundEngine& engine, size_t scene) {
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
				engine.midi_message(message, info);
				break;
			}
		}
		else {
			engine.midi_message(message, info);
		}
	}
}

void SoundEngineChannel::update_properties() {
	SoundEngineScene& scene = scenes[device->scene];
	submit_change(SoundEngineChannelProperty::pChannelActive, scene.active);
	submit_change(SoundEngineChannelProperty::pChannelVolume, volume);
	submit_change(SoundEngineChannelProperty::pChannelPanning, panning);
	submit_change(SoundEngineChannelProperty::pChannelSoundEngine, (int) engine_index);

	submit_change(SoundEngineChannelProperty::pChannelInputDevice, (int) scene.source.input);
	submit_change(SoundEngineChannelProperty::pChannelInputChannel, (int) scene.source.channel);
	submit_change(SoundEngineChannelProperty::pChannelStartNote, (int) scene.source.start_note);
	submit_change(SoundEngineChannelProperty::pChannelEndNote, (int) scene.source.end_note);
	submit_change(SoundEngineChannelProperty::pChannelStartVelocity, (int) scene.source.start_velocity);
	submit_change(SoundEngineChannelProperty::pChannelEndVelocity, (int) scene.source.end_velocity);
	submit_change(SoundEngineChannelProperty::pChannelOctave, scene.source.octave);
	submit_change(SoundEngineChannelProperty::pChannelTransferChannelAftertouch, scene.source.transfer_channel_aftertouch);
	submit_change(SoundEngineChannelProperty::pChannelTransferPitchBend, scene.source.transfer_pitch_bend);
	submit_change(SoundEngineChannelProperty::pChannelTransferCC, scene.source.transfer_cc);
	submit_change(SoundEngineChannelProperty::pChannelTransferProgChange, scene.source.transfer_prog_change);
	submit_change(SoundEngineChannelProperty::pChannelTransferOther, scene.source.transfer_other);

	submit_change(SoundEngineChannelProperty::pArpeggiatorOn, arp.on);
	submit_change(SoundEngineChannelProperty::pArpeggiatorPattern, arp.preset.pattern);
	submit_change(SoundEngineChannelProperty::pArpeggiatorOctaves, (int) arp.preset.octaves);
	submit_change(SoundEngineChannelProperty::pArpeggiatorStep, (int) arp.preset.value);
	submit_change(SoundEngineChannelProperty::pArpeggiatorHold, arp.preset.hold);
	submit_change(SoundEngineChannelProperty::pArpeggiatorBPM, (int) arp.metronome.get_bpm());
}

PropertyValue SoundEngineChannel::get(size_t prop, size_t sub_prop) {
	SoundEngineScene& scene = scenes[device->scene];
	PropertyValue value = {0};
	switch ((SoundEngineChannelProperty) prop) {
	case SoundEngineChannelProperty::pChannelActive:
		value.bval = scene.active;
		break;
	case SoundEngineChannelProperty::pChannelVolume:
		value.dval = volume;
		break;
	case SoundEngineChannelProperty::pChannelPanning:
		value.dval = panning;
		break;
	case SoundEngineChannelProperty::pChannelSoundEngine:
		value.ival = engine_index;
		break;
	case SoundEngineChannelProperty::pChannelInputDevice:
		value.ival = scene.source.input;
		break;
	case SoundEngineChannelProperty::pChannelInputChannel:
		value.ival = scene.source.channel;
		break;
	case SoundEngineChannelProperty::pChannelStartNote:
		value.ival = scene.source.start_note;
		break;
	case SoundEngineChannelProperty::pChannelEndNote:
		value.ival = scene.source.end_note;
		break;
	case SoundEngineChannelProperty::pChannelStartVelocity:
		value.ival = scene.source.start_velocity;
		break;
	case SoundEngineChannelProperty::pChannelEndVelocity:
		value.ival = scene.source.end_velocity;
		break;
	case SoundEngineChannelProperty::pChannelOctave:
		value.ival = scene.source.octave;
		break;
	case SoundEngineChannelProperty::pChannelTransferChannelAftertouch:
		value.bval = scene.source.transfer_channel_aftertouch;
		break;
	case SoundEngineChannelProperty::pChannelTransferPitchBend:
		value.bval = scene.source.transfer_pitch_bend;
		break;
	case SoundEngineChannelProperty::pChannelTransferCC:
		value.bval = scene.source.transfer_cc;
		break;
	case SoundEngineChannelProperty::pChannelTransferProgChange:
		value.bval = scene.source.transfer_prog_change;
		break;
	case SoundEngineChannelProperty::pChannelTransferOther:
		value.bval = scene.source.transfer_other;
		break;
	case SoundEngineChannelProperty::pArpeggiatorOn:
		value.bval = arp.on;
		break;
	case SoundEngineChannelProperty::pArpeggiatorPattern:
		value.ival = arp.preset.pattern;
		break;
	case SoundEngineChannelProperty::pArpeggiatorOctaves:
		value.ival = arp.preset.octaves;
		break;
	case SoundEngineChannelProperty::pArpeggiatorStep:
		value.ival = arp.preset.value;
		break;
	case SoundEngineChannelProperty::pArpeggiatorHold:
		value.bval = arp.preset.hold;
		break;
	case SoundEngineChannelProperty::pArpeggiatorBPM:
		value.ival = arp.metronome.get_bpm();
		break;
	}
	return value;
}

void SoundEngineChannel::set(size_t prop, PropertyValue value, size_t sub_prop) {
	SoundEngineScene& scene = scenes[device->scene];
	switch ((SoundEngineChannelProperty) prop) {
	case SoundEngineChannelProperty::pChannelActive:
		scene.active = value.bval;
		break;
	case SoundEngineChannelProperty::pChannelVolume:
		volume = value.dval;
		break;
	case SoundEngineChannelProperty::pChannelPanning:
		panning = value.dval;
		break;
	case SoundEngineChannelProperty::pChannelSoundEngine:
		engine_index = value.ival;
		break;
	case SoundEngineChannelProperty::pChannelInputDevice:
		scene.source.input= value.ival;
		break;
	case SoundEngineChannelProperty::pChannelInputChannel:
		scene.source.channel = value.ival;
		break;
	case SoundEngineChannelProperty::pChannelStartNote:
		scene.source.start_note = value.ival;
		break;
	case SoundEngineChannelProperty::pChannelEndNote:
		scene.source.end_note = value.ival;
		break;
	case SoundEngineChannelProperty::pChannelStartVelocity:
		scene.source.start_velocity = value.ival;
		break;
	case SoundEngineChannelProperty::pChannelEndVelocity:
		scene.source.end_velocity = value.ival;
		break;
	case SoundEngineChannelProperty::pChannelOctave:
		scene.source.octave = value.ival;
		break;
	case SoundEngineChannelProperty::pChannelTransferChannelAftertouch:
		scene.source.transfer_channel_aftertouch = value.bval;
		break;
	case SoundEngineChannelProperty::pChannelTransferPitchBend:
		scene.source.transfer_pitch_bend = value.bval;
		break;
	case SoundEngineChannelProperty::pChannelTransferCC:
		scene.source.transfer_cc = value.bval;
		break;
	case SoundEngineChannelProperty::pChannelTransferProgChange:
		scene.source.transfer_prog_change = value.bval;
		break;
	case SoundEngineChannelProperty::pChannelTransferOther:
		scene.source.transfer_other = value.bval;
		break;
	case SoundEngineChannelProperty::pArpeggiatorOn:
		arp.on = value.bval;
		break;
	case SoundEngineChannelProperty::pArpeggiatorPattern:
		arp.preset.pattern = static_cast<ArpeggiatorPattern>(value.ival);
		break;
	case SoundEngineChannelProperty::pArpeggiatorOctaves:
		arp.preset.octaves = value.ival;
		break;
	case SoundEngineChannelProperty::pArpeggiatorStep:
		arp.preset.value = value.ival;
		break;
	case SoundEngineChannelProperty::pArpeggiatorHold:
		arp.preset.hold = value.bval;
		break;
	case SoundEngineChannelProperty::pArpeggiatorBPM:
		arp.metronome.set_bpm(value.ival);
		break;
	}
}

void SoundEngineChannel::set_engine(ssize_t engine_index) {
	this->engine_index = engine_index;
}

ssize_t SoundEngineChannel::get_engine() {
	return engine_index;
}

inline SoundEngine* SoundEngineChannel::get_engine(std::vector<SoundEngineBank*>& engines, unsigned int channel) {
	ssize_t engine_index = this->engine_index;
	if (engine_index >= 0 && engine_index < (ssize_t) engines.size()) {
		return &engines[engine_index]->channel(channel);
	}
	return nullptr;
}

SoundEngineChannel::~SoundEngineChannel() {
	set_engine(-1);
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
		SoundEngine* engine = ch.get_engine(sound_engines, i);
		ch.process_sample(lsample, rsample, info, metronome, engine, scene);
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

std::vector<SoundEngineBank*> SoundEngineDevice::get_sound_engines() {
	return sound_engines;
}

void SoundEngineDevice::add_sound_engine(SoundEngineBank* engine) {
	sound_engines.push_back(engine);
}

void SoundEngineDevice::send(MidiMessage &message, SampleInfo& info) {
	SoundEngineChannel& ch = this->channels[message.channel];
	SoundEngine* engine = ch.get_engine(sound_engines, message.channel);
	if (engine) {
		ch.send(message, info, *engine, scene);
	}
}

PropertyValue SoundEngineDevice::get(size_t prop, size_t sub_prop) {
	PropertyValue val;
	switch ((SoundEngineProperty) prop) {
	case SoundEngineProperty::pEngineMetronomeOn:
		val.bval = play_metronome;
		break;
	case SoundEngineProperty::pEngineMetronomeBPM:
		val.ival = metronome.get_bpm();
		break;
	case SoundEngineProperty::pEngineVolume:
		val.dval = volume;
		break;
	}
	return val;
}

void SoundEngineDevice::set(size_t prop, PropertyValue val, size_t sub_prop) {
	switch ((SoundEngineProperty) prop) {
	case SoundEngineProperty::pEngineMetronomeOn:
		play_metronome = val.bval;
		break;
	case SoundEngineProperty::pEngineMetronomeBPM:
		 metronome.set_bpm(val.ival);
		break;
	case SoundEngineProperty::pEngineVolume:
		volume = val.dval;
		break;
	}
}

void SoundEngineDevice::update_properties() {
	submit_change(SoundEngineProperty::pEngineMetronomeOn, play_metronome);
	submit_change(SoundEngineProperty::pEngineMetronomeBPM, (int) metronome.get_bpm());
	submit_change(SoundEngineProperty::pEngineVolume, volume);
}

void SoundEngineDevice::apply_program(Program* program) {
	//Global
	metronome.set_bpm(program->metronome_bpm);
	scene = 0;
	//Channels
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		ChannelProgram& prog = program->channels[i];
		SoundEngineChannel& ch = channels[i];
		//Reset old engine
		SoundEngine* engine = ch.get_engine(sound_engines, i);
		if (engine) {
			engine->apply_program(nullptr);
		}

		ch.set_engine(prog.engine_index);
		ch.volume = prog.volume;
		ch.panning = prog.panning;
		ch.scenes = prog.scenes;
		ch.arp.on = prog.arp_on;
		ch.arp.metronome.set_bpm(prog.arpeggiator_bpm);
		ch.arp.preset = prog.arpeggiator;

		//Engine
		engine = ch.get_engine(sound_engines, i);
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
		prog.engine_index = ch.get_engine();
		prog.volume = ch.volume;
		prog.panning = ch.panning;
		prog.scenes = ch.scenes;
		prog.arp_on = ch.arp.on;
		prog.arpeggiator_bpm = ch.arp.metronome.get_bpm();
		prog.arpeggiator = ch.arp.preset;

		//Engine
		SoundEngine* engine = ch.get_engine(sound_engines, i);
		if (engine) {
			engine->save_program(&prog.engine_program);
		}
	}
}

SoundEngineDevice::~SoundEngineDevice() {
	//Clear channels
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i].set_engine(-1);
	}
	//Delete engines
	for (SoundEngineBank* engine : sound_engines) {
		delete engine;
	}
	sound_engines.clear();
}
