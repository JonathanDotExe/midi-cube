/*
 * midi_jammer.h
 *
 *  Created on: 22 Dec 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_PLUGINS_SEQUENCER_MIDI_JAMMER_H_
#define MIDICUBE_PLUGINS_SEQUENCER_MIDI_JAMMER_H_

#include "../../framework/core/plugin.h"

#define MIDI_JAMMER_IDENTIFIER "midicube_midi_jammer"

class MidiJammer : public PluginInstance {
public:
	MidiJammer(PluginHost& h, Plugin& p) : PluginInstance(h, p) {

	}
	void apply_program(PluginProgram *prog);
	Menu* create_menu();
	void process(const SampleInfo &info);
	void save_program(PluginProgram **prog);
	void recieve_midi(const MidiMessage &message, const SampleInfo &info);
	~MidiJammer() {

	}
};

class MidiJammerPlugin : public Plugin {
public:
	MidiJammerPlugin() : Plugin({
		"Midi Jammer",
		MIDI_JAMMER_IDENTIFIER,
		PluginType::PLUGIN_TYPE_SEQUENCER,
		0,
		0,
		true,
		true
	}){

	}

	PluginProgram* create_program();
	PluginInstance* create(PluginHost *host);
	virtual ~MidiJammerPlugin() {

	}
};

#endif /* MIDICUBE_PLUGINS_SEQUENCER_MIDI_JAMMER_H_ */
