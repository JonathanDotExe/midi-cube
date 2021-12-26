/*
 * note_tester.h
 *
 *  Created on: 22 Dec 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_PLUGINS_SEQUENCER_NOTE_TESTER_H_
#define MIDICUBE_PLUGINS_SEQUENCER_NOTE_TESTER_H_

#include "../../framework/core/plugin.h"

#define NOTE_TESTER_IDENTIFIER "midicube_note_tester"

class NoteTester : public PluginInstance {
private:
	unsigned int note = 60;

public:
	NoteTester(PluginHost& h, Plugin& p) : PluginInstance(h, p) {

	}
	void apply_program(PluginProgram *prog);
	Menu* create_menu();
	void process(const SampleInfo &info);
	void save_program(PluginProgram **prog);
	void recieve_midi(const MidiMessage &message, const SampleInfo &info);
	~NoteTester() {

	}
};

class NoteTesterPlugin : public Plugin {
public:
	NoteTesterPlugin() : Plugin({
		"Note Tester",
		NOTE_TESTER_IDENTIFIER,
		PluginType::PLUGIN_TYPE_SEQUENCER,
		0,
		0,
		false,
		true
	}){

	}

	PluginProgram* create_program();
	PluginInstance* create(PluginHost *host);
	virtual ~NoteTesterPlugin() {

	}
};

#endif /* MIDICUBE_PLUGINS_SEQUENCER_NOTE_TESTER_H_ */
