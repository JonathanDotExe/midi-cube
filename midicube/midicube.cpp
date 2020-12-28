/*
 * midicube.cpp
 *
 *  Created on: Oct 3, 2020
 *      Author: jojo
 */

#include "midicube.h"
#include "soundengine/engines.h"

static void process_func(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, void* user_data) {
	((MidiCube*) user_data)->process(channels, info);
}

MidiCube::MidiCube() : tasks(64){
	audio_handler.set_sample_callback(&process_func, this);
}

void MidiCube::init() {
	//MIDI Inputs
	//Input-Devices
	MidiInput input_dummy;
	size_t n_ports = input_dummy.available_ports();
	for (size_t i = 0; i < n_ports; ++i) {
		MidiInput* input = new MidiInput();
		std::string name;
		try {
			name = input->port_name(i);
			input->open(i);
		}
		catch (MidiException& exc) {
			delete input;
			input = nullptr;
		}
		if (input != nullptr) {
			inputs.push_back({input, name});
			size_t index = inputs.size() - 1;
			input->set_callback([index, this](double delta, MidiMessage& msg) {
				midi_callback(msg, index);
			});
		}
	}
	//Sound Engines
	fill_sound_engine_device(&engine);
	//Synth presets
	SoundEngineBank* bank = engine.get_sound_engines().at(2);
	apply_preset(UNISON_SAWS, static_cast<AnalogSynth&>(bank->channel(0)).preset);
	apply_preset(PULSE_BASS, static_cast<AnalogSynth&>(bank->channel(1)).preset);
	apply_preset(STRONG_PAD, static_cast<AnalogSynth&>(bank->channel(2)).preset);
	apply_preset(DELAY_CHORDS, static_cast<AnalogSynth&>(bank->channel(3)).preset);
	apply_preset(POLY_SWEEP, static_cast<AnalogSynth&>(bank->channel(4)).preset);
	apply_preset(FM_E_PIANO, static_cast<AnalogSynth&>(bank->channel(5)).preset);
	apply_preset(POLY_SWEEP, static_cast<AnalogSynth&>(bank->channel(6)).preset);
	apply_preset(BRASS_PAD, static_cast<AnalogSynth&>(bank->channel(7)).preset);
	apply_preset(FM_KALIMBA, static_cast<AnalogSynth&>(bank->channel(8)).preset);
	//Default setting
	SoundEngineBank* bank2 = engine.get_sound_engines().at(2);
	static_cast<BaseSoundEngine&>(bank2->channel(1)).sustain = false;
	Arpeggiator& arp = engine.get_channel(1).arpeggiator();
	arp.on = true;
	arp.preset.octaves = 3;
	arp.preset.pattern = ArpeggiatorPattern::UP;
	arp.preset.value = 1;
	arp.metronome.set_bpm(440);
	//Init audio
	audio_handler.init();
}

void MidiCube::process(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	//Changes
	PropertyChange change;
	while (changes.pop(change)) {
		change.holder->set(change.property, change.value);
	}
	//Tasks
	std::function<void ()>* task;
	while (tasks.pop(task)) {
		(*task)();
		delete task;	//TODO make own garbage collector task
	}
	//Process
	engine.process_sample(channels, info);
}

std::vector<MidiCubeInput> MidiCube::get_inputs() {
	return inputs;
}

void MidiCube::midi_callback(MidiMessage& message, size_t input) {
	MessageType t = message.get_message_type();
	SampleInfo info = audio_handler.sample_info();
	for (size_t i = 0; i < channels.size(); ++i) {
		ChannelSource& s = channels[i];
		if (s.input == static_cast<ssize_t>(input) && s.channel == message.get_channel()) {
			bool pass = true;
			MidiMessage msg = message;
			switch (t) {
			case MessageType::NOTE_OFF:
			case MessageType::NOTE_ON:
			case MessageType::POLYPHONIC_AFTERTOUCH:
				pass = s.start_note <= message.get_note() && s.end_note >= message.get_note();
				if (s.octave) {
					msg.set_note(msg.get_note() + s.octave * 12);
				}
				break;
			case MessageType::CONTROL_CHANGE:
				pass = s.transfer_cc;
				break;
			case MessageType::PROGRAM_CHANGE:
				pass = s.transfer_prog_change;
				break;
			case MessageType::MONOPHONIC_AFTERTOUCH:
				pass = s.transfer_channel_aftertouch;
				break;
			case MessageType::PITCH_BEND:
				pass = s.transfer_pitch_bend;
				break;
			case MessageType::SYSEX:
				pass = s.transfer_other;
				break;
			case MessageType::INVALID:
				pass = false;
			}
			//Apply binding
			if (pass) {
				msg.set_channel(i);
				engine.send(msg, info);
			}
		}
	}
}

void MidiCube::run_task(std::function<void ()> task) {
	tasks.push(new std::function<void ()>(task));
}

void MidiCube::perform_change(PropertyChange change) {
	changes.push(change);
}

MidiCube::~MidiCube() {
	audio_handler.close();
	for (MidiCubeInput in : inputs) {
		delete in.in;
	}
	inputs.clear();
	//TODO memory leak possible maybe?
	std::function<void ()>* task;
	while (tasks.pop(task)) {
		delete task;
	}
}
