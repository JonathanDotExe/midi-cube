/*
 * note_tester.cpp
 *
 *  Created on: 22 Dec 2021
 *      Author: jojo
 */

#include "note_tester.h"


void NoteTester::apply_program(PluginProgram *prog) {
}

Menu* NoteTester::create_menu() {
	return nullptr;
}

void NoteTester::process(const SampleInfo &info) {
	if (host_metronome.is_beat(info.sample_time, info.sample_rate, 1)) {
		MidiMessage msg;
		msg.type = MessageType::NOTE_OFF;
		msg.set_note(note);
		send_midi(msg, info);
		msg.type = MessageType::NOTE_ON;
		msg.set_velocity(127);
		send_midi(msg, info);
	}
}

void NoteTester::save_program(PluginProgram **prog) {
}

void NoteTester::recieve_midi(const MidiMessage &message, const SampleInfo &info) {
}

PluginProgram* NoteTesterPlugin::create_program() {
	return nullptr;
}

PluginInstance* NoteTesterPlugin::create(PluginHost *host) {
	return new NoteTester(*host, *this);
}
