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
#include <unordered_map>

#define ARPEGGIATOR_IDENTIFIER "midicube_arpeggiator"

class ArpeggiatorProgram : public PluginProgram {
public:
	virtual void load(boost::property_tree::ptree tree);
	virtual std::string get_plugin_name();
	virtual boost::property_tree::ptree save();
};

class ArpeggiatorInstance : public PluginInstance {

public:

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
		PluginType::SEQUENCER,
		0,
		2,
		true,
		false
	}){

	}

	virtual PluginProgram* create_program();
	virtual PluginInstance* create(PluginHost *host);
	virtual ~ArpeggiatorPlugin() {

	}
};


#endif /* MIDICUBE_SEQUENCER_ARPEGGIATOR_H_ */
