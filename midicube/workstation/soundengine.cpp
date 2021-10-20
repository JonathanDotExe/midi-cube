/*
 * soundengine.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "soundengine.h"
#include "midicube.cpp"

#include <algorithm>


//SoundEngineChannel
SoundEngineChannel::SoundEngineChannel() {
	binder.add_binding(&volume);
	binder.add_binding(&panning);
}

void SoundEngineChannel::init_device(SoundEngineDevice* device) {
	if (!this->device) {
		this->device = device;
		engine.init(this);
		for (size_t i = 0; i < effects.size(); ++i) {
			effects[i].init(this);
		}

		binder.init(&device->binding_handler);
	}
}

void SoundEngineChannel::process_sample(double& lsample, double& rsample, double* inputs, const size_t input_count, const SampleInfo& info) {
	size_t scene = device->scene;
	//Properties
	PluginInstance* engine = this->engine.get_plugin();
	if (engine) {
		SoundEngineScene& s = scenes[scene];
		//Pitch and Sustain
		//FIXME customize environment
		/*if (!s.sustain) {
			env.sustain = false;
		}
		if (!s.pitch_bend) {
			env.pitch_bend = 1;
		}*/

		if (s.active) { //FIXME maintain when notes pressed
			//Process
			//TODO take input
			engine->take_inputs(inputs, input_count);
			engine->process(info);
			engine->playback_outputs_stereo(lsample, rsample);
		}
		//Effects
		for (size_t i = 0; i < CHANNEL_INSERT_EFFECT_AMOUNT; ++i) {
			if (effects[i].get_plugin()) {
				effects[i].get_plugin()->take_input_stereo_and_inputs(lsample, rsample, inputs, input_count);
				effects[i].get_plugin()->process(info);
				effects[i].get_plugin()->playback_outputs_stereo(lsample, rsample);
			}
		}
		//Pan
		lsample *= (1 - fmax(0, panning));
		rsample *= (1 - fmax(0, -panning));
		//Playback
		lsample *= volume;
		rsample *= volume;
	}
}

void SoundEngineChannel::send(const MidiMessage &message, const SampleInfo& info, void* src) {
	size_t scene = device->scene;
	PluginInstance* engine = this->engine.get_plugin();
	if (scenes[scene].active /* || (status.pressed_notes && message.type != MessageType::NOTE_ON)*/) { //FIXME send when channel is deactivated
		//Recieve midi
		PluginInstance* seq = sequencer.get_plugin();
		if (seq == nullptr || src == seq) {
			//Aftertouch
			if (message.type == MessageType::MONOPHONIC_AFTERTOUCH) {
				this->info.aftertouch = message.monophonic_aftertouch()/127.0;
			}
			engine->recieve_midi(message, info); //FIXME transpose not working
		}
		//Sequencer
		else if (seq != nullptr){
			seq->recieve_midi(message, info);
		}
	}
}

PluginInstance* SoundEngineChannel::get_engine() {
	return engine.get_plugin();
}

void SoundEngineChannel::set_engine(Plugin* engine) {
	this->engine.set_plugin(engine);
}

SoundEngineChannel::~SoundEngineChannel() {

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

SpinLock& SoundEngineChannel::get_lock() {
	return device->get_cube()->lock;
}

//SoundEngineDevice
SoundEngineDevice::SoundEngineDevice() : metronome(120){
	host.init(this);
	metronome.init(0);
}

void SoundEngineDevice::process_sample(double& lsample, double& rsample, double* inputs, const size_t input_count, const SampleInfo& info) {
	//Channels
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		double l = 0;
		double r = 0;
		SoundEngineChannel& ch = this->channels[i];
		ch.process_sample(l, r, inputs, input_count, info);

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
		if (effect.effect.get_plugin()) {
			effect.effect.get_plugin()->take_input_stereo_and_inputs(effect.lsample, effect.rsample, inputs, input_count);
			effect.effect.get_plugin()->process(info);
			effect.effect.get_plugin()->playback_outputs_stereo(l, r);
		}
		effect.lsample = 0;
		effect.rsample = 0;

		if (effect.next_effect > i && effect.next_effect < SOUND_ENGINE_MASTER_EFFECT_AMOUNT) {
			effects[effect.next_effect].lsample += l;
			effects[effect.next_effect].rsample += r;
		}
		else {
			lsample += l;
			rsample += r;
		}
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

void SoundEngineDevice::send(MidiMessage &message, size_t input, MidiSource& source, SampleInfo& info) {
	bool updated = false;
	double pitch;

	//Global values
	switch (message.type) {
	case MessageType::MONOPHONIC_AFTERTOUCH:
		break;
	case MessageType::POLYPHONIC_AFTERTOUCH:
		break;
	case MessageType::NOTE_ON:
		break;
	case MessageType::NOTE_OFF:
		break;
	case MessageType::CONTROL_CHANGE:
		env.ccs[message.control()] = message.value()/127.0;
		//Sustain
		if (message.control() == sustain_control) {
			bool new_sustain = message.value() != 0;
			if (new_sustain != env.sustain) {
				if (new_sustain) {
					env.sustain_time = info.time;
				}
				else {
					env.sustain_release_time = info.time;
				}
				env.sustain = new_sustain;
			}
		}
		//Update scene
		for (size_t i = 0; i < SOUND_ENGINE_SCENE_AMOUNT; ++i) {
			if (scene_ccs[i] == message.control()) {
				scene = i;
				updated = true;
			}
		}
		//Binding handler
		updated = binding_handler.on_cc(message.control(), message.value()/127.0) || updated;
		break;
	case MessageType::PROGRAM_CHANGE:
		break;
	case MessageType::PITCH_BEND:
		env.pitch_bend_percent = message.pitch_bend()/8192.0 - 1.0;
		pitch = env.pitch_bend_percent * 2;
		env.pitch_bend = note_to_freq_transpose(pitch);
		break;
	case MessageType::SYSEX:
		//Clock
		if (source.clock_in) {
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
		break;
	case MessageType::INVALID:
		break;
	}

	//Channels
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		SoundEngineChannel& channel = channels[i];
		ChannelSource& s = channel.scenes[scene].source;
		if (s.input < 0 || input == static_cast<size_t>(s.input)) {
			//Filter
			bool pass = true;
			switch (message.type) {
			case MessageType::NOTE_OFF:
				break;
			case MessageType::NOTE_ON:
				pass = message.velocity() >= s.start_velocity && message.velocity() <= s.end_velocity;
				/* no break */
			case MessageType::POLYPHONIC_AFTERTOUCH:
				pass = pass && message.note() >= s.start_note && message.note() <= s.end_note;
				break;
			case MessageType::MONOPHONIC_AFTERTOUCH:
				pass = s.transfer_channel_aftertouch;
				break;
			case MessageType::CONTROL_CHANGE:
				pass = s.transfer_cc;	//FIXME global ccs are updated anyways
				break;
			case MessageType::PROGRAM_CHANGE:
				pass = s.transfer_prog_change;
				break;
			case MessageType::PITCH_BEND:
				pass = s.transfer_pitch_bend;
				break;
			case MessageType::SYSEX:
				pass = s.transfer_other; //TODO remove probably
				break;
			case MessageType::INVALID:
				break;
			}
			//Send
			if (pass) {
				channel.send(message, info, nullptr);
			}
		}
	}
	//TODO refresh GUI
}


void SoundEngineDevice::apply_program(Program* program) {
	//Global
	metronome.set_bpm(program->metronome_bpm);
	scene = 0;
	//Channels
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		ChannelProgram& prog = program->channels[i];
		SoundEngineChannel& ch = channels[i];

		ch.engine.load(prog.engine_program, &cube->plugin_mgr);
		ch.sequencer.load(prog.sequencer_program, &cube->plugin_mgr);
		ch.volume = prog.volume;
		ch.panning = prog.panning;
		ch.scenes = prog.scenes;
		ch.polyphony_limit = prog.polyphony_limit;

		//Engine
		ch.master_send = prog.send_master;
		//Effects
		for (size_t i = 0; i < CHANNEL_INSERT_EFFECT_AMOUNT; ++i) {
			PluginSlotProgram& p = prog.effects[i];
			ch.effects[i].load(p, &cube->plugin_mgr);
		}
	}
	//Master Effects
	for (size_t i = 0; i < SOUND_ENGINE_MASTER_EFFECT_AMOUNT; ++i) {
		MasterEffectProgram& p = program->effects[i];
		effects[i].load(p, &cube->plugin_mgr);
	}
}

void SoundEngineDevice::save_program(Program* program) {
	//Global
	program->metronome_bpm = metronome.get_bpm();
	//Channels
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		ChannelProgram& prog = program->channels[i];
		SoundEngineChannel& ch = channels[i];
		ch.engine.save(prog.engine_program);
		ch.sequencer.save(prog.sequencer_program);
		prog.volume = ch.volume;
		prog.panning = ch.panning;
		prog.scenes = ch.scenes;
		prog.polyphony_limit = ch.polyphony_limit;

		//Engine
		prog.send_master = ch.master_send;
		//Effects
		for (size_t j = 0; j < CHANNEL_INSERT_EFFECT_AMOUNT; ++j) {
			PluginSlotProgram& p = prog.effects[j];
			ch.effects[i].save(p);
		}
	}
	//Master Effects
	for (size_t i = 0; i < SOUND_ENGINE_MASTER_EFFECT_AMOUNT; ++i) {
		MasterEffectProgram& p = program->effects[i];
		effects[i].save(p);
	}
}

SoundEngineDevice::~SoundEngineDevice() {
	//Clear channels
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i].set_engine(nullptr);
		for (size_t j = 0; j < channels[i].effects.size(); ++j) {
			channels[i].effects[j].set_plugin(nullptr);
		}
	}
	for (size_t j = 0; j < effects.size(); ++j) {
		effects[j].effect.set_plugin(nullptr);
	}
}

bool SoundEngineChannel::is_sustain() const {
	return scenes[device->scene].sustain;
}

void SoundEngineChannel::set_sustain(bool sustain) {
	scenes[device->scene].sustain = sustain;
}

bool SoundEngineChannel::is_pitch_bend() const {
	return scenes[device->scene].pitch_bend;
}

void SoundEngineChannel::set_pitch_bend(bool pitch_bend) {
	scenes[device->scene].pitch_bend = pitch_bend;
}

void SoundEngineDevice::init(MidiCube *cube) {
	if (this->cube) {
		throw "MidiCube already initialized";
	}
	else {
		this->cube = cube;
		for (size_t i = 0; i < this->effects.size(); ++i) {
			effects[i].effect.init(&host);
		}
		for (size_t i = 0; i < this->channels.size(); ++i) {
			SoundEngineChannel& ch = this->channels[i];
			ch.init_device(this);
		}
	}
}

void SoundEngineChannel::recieve_midi(const MidiMessage &message,
		const SampleInfo &info, void* src) {
	this->send(message, info, src);
}

MidiBindingHandler* SoundEngineChannel::get_binding_handler() {
	return &device->binding_handler;
}

Plugin* SoundEngineChannel::get_plugin(std::string identifier) {
	return device->get_cube()->plugin_mgr.get_plugin(identifier);
}

const Metronome& SoundEngineChannel::get_metronome() {
	return device->metronome;
}

const KeyboardEnvironment& SoundEngineChannel::get_environment() {
	return device->env; //TODO channel aftertouch
}

//SoundEngineDeviceHost
void SoundEngineDeviceHost::recieve_midi(const MidiMessage &message,
		const SampleInfo &info, void* src) {
	//TODO
}

Plugin* SoundEngineDeviceHost::get_plugin(std::string identifier) {
	return device->get_cube()->plugin_mgr.get_plugin(identifier);
}

MidiBindingHandler* SoundEngineDeviceHost::get_binding_handler() {
	return &device->binding_handler;
}

const Metronome& SoundEngineDeviceHost::get_metronome() {
	return device->metronome;
}

const KeyboardEnvironment& SoundEngineDeviceHost::get_environment() {
	return device->env; //TODO channel aftertouch
}

void SoundEngineDeviceHost::init(SoundEngineDevice *device) {
	if (this->device) {
		throw "Device already intialized!";
	}
	this->device = device;
}

SpinLock& SoundEngineDeviceHost::get_lock() {
	return this->device->get_cube()->lock;
}