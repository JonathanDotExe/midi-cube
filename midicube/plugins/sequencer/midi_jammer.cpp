/*
 * midi_jammer.cpp
 *
 *  Created on: 22 Dec 2021
 *      Author: jojo
 */

#include "midi_jammer.h"


void MidiJammer::apply_program(PluginProgram *prog) {
}

Menu* MidiJammer::create_menu() {
	return nullptr;
}

void MidiJammer::process(const SampleInfo &info) {

}

void MidiJammer::save_program(PluginProgram **prog) {
}

void MidiJammer::recieve_midi(const MidiMessage &message, const SampleInfo &info) {
}

PluginProgram* MidiJammerPlugin::create_program() {
	return nullptr;
}

PluginInstance* MidiJammerPlugin::create(PluginHost *host) {
	return new MidiJammer(*host, *this);
}
