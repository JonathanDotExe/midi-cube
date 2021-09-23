/*
 * plugin.h
 *
 *  Created on: 16 Sep 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_PLUGIN_H_
#define MIDICUBE_PLUGIN_H_

#include <string>
#include "midi.h"
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

enum class PluginType {
	SOUND_ENGINE, EFFECT, SEQUENCER, OTHER
};

struct PluginInfo {
	std::string name = "Plugin";
	std::string identifier_name = "plugin";
	unsigned int input_channels = 0;
	unsigned int output_channels = 0;
	bool input_midi = false;
	bool output_midi = false;
};

class PluginProgram {
public:
	virtual void load(pt::ptree tree) = 0;

	virtual pt::ptree save() = 0;

	virtual ~PluginProgram() {

	}
};

class PluginHost {

public:

	virtual KeyboardEnvironment get_environment();

	virtual void recieve_midi(const MidiMessage& message, const SampleInfo& info) = 0;

	virtual ~PluginHost() {

	}

};

class PluginInstance;

class Plugin {

public:

	const PluginInfo info;

	Plugin(PluginInfo i) : info(i) {

	}

	virtual PluginInstance* create(PluginHost* host) = 0;

	virtual PluginProgram* create_program() = 0;

	virtual ~Plugin() {

	}

};

class PluginInstance {

private:
	PluginHost& host;
	Plugin& plugin;

protected:

	void send_midi(const MidiMessage& msg, const SampleInfo& info) {
		host.recieve_midi(msg, info);
	}

public:
	double** inputs;
	double** outputs;

	PluginInstance(PluginHost& h, Plugin& p) : host(h), plugin(p) {
		inputs = new double[p.info.input_channels];
		outputs = new double[p.info.output_channels];
	}

	virtual void process(const SampleInfo& info) = 0;

	virtual void recieve_midi(MidiMessage& message, const SampleInfo& info) = 0;

	virtual void apply_program(PluginProgram* prog) = 0;

	virtual void save_program(PluginProgram** prog) = 0;

	virtual ~PluginInstance() {
		delete inputs;
		delete outputs;
	}

	PluginHost& get_host() const {
		return host;
	}

	Plugin& get_plugin() const {
		return plugin;
	}
};

#endif
