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

	virtual std::string get_plugin_name() = 0;

	virtual ~PluginProgram() {

	}
};

class Plugin;

class PluginHost {

public:

	virtual KeyboardEnvironment get_environment();

	virtual void recieve_midi(const MidiMessage& message, const SampleInfo& info) = 0;

	virtual Plugin* get_plugin(std::string identifier);

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
	double* inputs;
	double* outputs;

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

class PluginManager {
private:
	std::unordered_map<std::string, Plugin*> plugins;

public:

	Plugin* get_plugin(std::string identifier) {
		return plugins[identifier];
	}

};

class PluginSlotProgram {
private:
	PluginProgram* program = nullptr;

public:
	PluginSlotProgram() {

	}

	void save_program(PluginInstance* plugin) {
		if (plugin) {
			plugin->save_program(&program); //FIXME ensure deletion of pointer in save_program
		}
		else {
			delete program;
			program = nullptr;
		}
	}

	PluginProgram* get_program() {
		return program;
	}

	void load(pt::ptree tree, PluginManager* mgr) {
		Plugin* plugin = mgr->get_plugin(tree.get("plugin", ""));
		if (plugin) {
			if (program) {
				delete program;
			}
			program = plugin->create_program();
			if (tree.get_child_optional("data")) {
				program->load(tree.get_child("data"));
			}
		}
	}

	pt::ptree save() {
		pt::ptree tree;
		tree.put("plugin", program ? program->get_plugin_name() : nullptr);
		if (program) {
			tree.add_child("data", program->save());
		}
		return tree;
	}

	~PluginSlotProgram() {
		delete program;
	}

};

class PluginSlot {

private:
	PluginHost* host = nullptr;
	PluginInstance* plugin = nullptr;

public:

	void init(PluginHost* host) {
		if (this->host) {
			throw "Host already initialised!";
		}
		this->host = host;
	}

	PluginInstance* get_plugin() const {
		return plugin;
	}

	void set_plugin(Plugin* plugin) {
		if (this->plugin) {
			delete this->plugin;
			this->plugin = nullptr;
		}
		if (plugin) {
			this->plugin = plugin->create(host);
		}
	}

	void load(PluginSlotProgram& prog, PluginManager* mgr) {
		if (prog.get_program()) {
			Plugin* p = mgr->get_plugin(prog.get_program()->get_plugin_name());
			if (p) {
				set_plugin(p);
				plugin->apply_program(prog.get_program());
			}
			else {
				set_plugin(nullptr);
			}
		}
		else {
			set_plugin(nullptr);
		}
	}

	void save(PluginSlotProgram& prog) {
		prog.save_program(plugin);
	}

	~PluginSlot() {
		delete plugin;
	}


};

#endif
