/*
 * midi_jammer.cpp
 *
 *  Created on: 22 Dec 2021
 *      Author: jojo
 */

#include "midi_jammer.h"


void MidiJammerBeat::reset() {
	messages.clear();
	messages.reserve(256);
}

void MidiJammerChannel::initialize_bars(unsigned int beats_per_bar) {
	bars = bars_setting;
	total_beats = bars * beats_per_bar;
	for (unsigned int i = bars; i < total_beats; ++i) {

	}
}


void MidiJammer::apply_program(PluginProgram *prog) {
}

Menu* MidiJammer::create_menu() {
	return nullptr;
}

void MidiJammer::process(const SampleInfo &info) {

}

void MidiJammer::save_program(PluginProgram **prog) {
}

void MidiJammer::initialize_bars() {
}

void MidiJammer::recieve_midi(const MidiMessage &message, const SampleInfo &info) {
}

PluginProgram* MidiJammerPlugin::create_program() {
	return nullptr;
}

PluginInstance* MidiJammerPlugin::create(PluginHost *host) {
	return new MidiJammer(*host, *this);
}

