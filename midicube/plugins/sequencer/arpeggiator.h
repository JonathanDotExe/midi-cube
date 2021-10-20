/*
 * arpeggiator.h
 *
 *  Created on: Oct 14, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_SEQUENCER_ARPEGGIATOR_H_
#define MIDICUBE_SEQUENCER_ARPEGGIATOR_H_


#include "../framework/core/plugin.h"
#include "../framework/util/audiofile.h"
#include "../framework/util/voice.h"
#include <unordered_map>

#define ARPEGGIATOR_IDENTIFIER "midicube_arpeggiator"
#define ARPEGGIATOR_POLYPHONY 30

enum ArpeggiatorPattern {
	ARP_UP, ARP_DOWN, ARP_RANDOM, ARP_UP_DOWN, ARP_DOWN_UP
};

struct ArpeggiatorPreset {
	ArpeggiatorPattern pattern;
	unsigned int octaves = 1;
	unsigned int value = 1;
	bool hold = false;
	bool repeat_edges = false;
	bool kb_sync = true;
	bool play_duplicates = false;
	bool master_sync = false;
	bool sustain = false;
};

class Arpeggiator {

private:
	unsigned int curr_note = 0;
	std::size_t data_index = 0;
	std::size_t note_index = 0;
	bool restart = true;
	bool second = false;
	VoiceManager<TriggeredNote, ARPEGGIATOR_POLYPHONY> note;

public:
	bool on = true;
	ArpeggiatorPreset preset;
	Metronome metronome;

	Arpeggiator();

	void apply(const SampleInfo& info, const Metronome& master, std::function<void(const SampleInfo&, unsigned int, double)> press, std::function<void(const SampleInfo&, unsigned int, double)> release, bool sustain);

	void press_note(const SampleInfo& info, unsigned int note, double velocity, bool sustain);

	void release_note(const SampleInfo& info, unsigned int note, bool sustain);

};

class ArpeggiatorProgram : public PluginProgram {
public:
	ArpeggiatorPreset preset;
	unsigned int bpm = 120;
	bool on;

	void load(boost::property_tree::ptree tree);
	std::string get_plugin_name();
	boost::property_tree::ptree save();
};

class ArpeggiatorInstance : public PluginInstance {
public:
	Arpeggiator arp;

	ArpeggiatorInstance(PluginHost& h, Plugin& p);
	void apply_program(PluginProgram *prog);
	void process(const SampleInfo &info);
	void save_program(PluginProgram **prog);
	void recieve_midi(const MidiMessage &message, const SampleInfo &info);
	~ArpeggiatorInstance();
};

class ArpeggiatorPlugin : public Plugin {
public:
	ArpeggiatorPlugin() : Plugin({
		"Arpeggiator",
		ARPEGGIATOR_IDENTIFIER,
		PluginType::PLUGIN_TYPE_SEQUENCER,
		0,
		0,
		true,
		true
	}){

	}

	virtual PluginProgram* create_program();
	virtual PluginInstance* create(PluginHost *host);
	virtual ~ArpeggiatorPlugin() {

	}
};


#endif /* MIDICUBE_SEQUENCER_ARPEGGIATOR_H_ */
