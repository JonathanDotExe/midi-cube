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


enum MidiJammerAction {
	MIDI_JAMMER_NONE, MIDI_JAMMER_PLAY_NEXT, MIDI_JAMMER_STOP_NEXT
};

struct MidiJammerMessage {
	MidiMessage msg;
	double time = 0;
};

class MidiJammerBeat {
	std::vector<MidiJammerMessage> messages;
};

class MidiJammerChannel {
private:
	bool play = false;
	unsigned int bars = 0;
	std::vector<MidiJammerBeat> beats;

public:
	MidiJammerAction action = MIDI_JAMMER_NONE;

	unsigned int quantize_step = 16;
	double quantize = 0.0;
};

class MidiJammer : public PluginInstance {
private:
	int beats_per_bar = 4;

public:
	std::array<MidiJammerChannel, SOUND_ENGINE_MIDI_CHANNELS> channels;

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
