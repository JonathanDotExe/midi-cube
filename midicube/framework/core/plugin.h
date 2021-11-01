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
#include "audio.h"
#include "../dsp/envelope.h"
#include "../data/binding.h"
#include "../util/util.h"
#include "../gui/core.h"
//#include "../gui/core.h"
#include <map>
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

enum PluginType {
	PLUGIN_TYPE_SOUND_ENGINE, PLUGIN_TYPE_EFFECT, PLUGIN_TYPE_SEQUENCER, PLUGIN_TYPE_OTHER
};

struct PluginInfo {
	std::string name = "Plugin";
	std::string identifier_name = "plugin";
	PluginType type = PluginType::PLUGIN_TYPE_OTHER;
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

	virtual const KeyboardEnvironment& get_environment() = 0;

	virtual const Metronome& get_metronome() = 0;

	virtual SpinLock& get_lock() = 0;

	virtual MidiBindingHandler* get_binding_handler() = 0;

	virtual void recieve_midi(const MidiMessage& message, const SampleInfo& info, void* source) = 0;

	virtual Plugin* get_plugin(std::string identifier) = 0;

	virtual void notify_property_update(void* source, void* prop) = 0;

	virtual int get_transpose() = 0;

	virtual ~PluginHost() {

	}

};

class PluginInstance;

//TODO use locks in all plugins
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
		host.recieve_midi(msg, info, this);
	}

public:
	double* inputs;
	double* outputs;

	PluginInstance(PluginHost& h, Plugin& p) : host(h), plugin(p) {
		inputs = new double[p.info.input_channels];
		outputs = new double[p.info.output_channels];
	}

	virtual void process(const SampleInfo& info) = 0;

	virtual void recieve_midi(const MidiMessage& message, const SampleInfo& info) = 0;

	virtual void apply_program(PluginProgram* prog) = 0;

	virtual void save_program(PluginProgram** prog) = 0;

	virtual ViewController* create_view() {
		return nullptr;
	}

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

	inline SpinLock& get_lock() const {
		return host.get_lock();
	}

	inline void notify_property_update(void* prop) {
		get_host().notify_property_update(this, prop);
	}

	inline void take_input_mono(double sample) {
		const size_t ins = plugin.info.input_channels;
		for (size_t i = 0; i < ins; ++i) {
			inputs[i] = sample;
		}
	}

	inline void take_input_stereo(double lsample, double rsample) {
		const size_t ins = plugin.info.input_channels;
		if (ins > 1) {
			for (size_t i = 0; i < ins; ++i) {
				for (size_t i = 0; i < ins; ++i) {
					if (i % 2 == 0) {
						inputs[i] = lsample;
					}
					else {
						inputs[i] = rsample;
					}
				}
			}
		}
		else if (ins == 1){
			inputs[0] = rsample + lsample;
		}
	}

	inline void take_input_stereo_and_inputs(double lsample, double rsample, double* inputs, const size_t input_count) {
		const size_t ins = plugin.info.input_channels;
		if (ins > 1) {
			this->inputs[0] = lsample;
			this->inputs[1] = rsample;
			const size_t mic_inputs = ins - 2;
			if (mic_inputs > 0) {
				for (size_t i = 0; i < input_count; ++i) {
					this->inputs[2 + (i % mic_inputs)] = inputs[i];
				}
			}
		}
		else if (ins == 1){
			this->inputs[0] = rsample + lsample;
		}
	}

	inline void take_inputs(double* inputs, const size_t input_count) {
		const size_t ins = plugin.info.input_channels;
		if (ins > 0) {
			for (size_t i = 0; i < input_count; ++i) {
				this->inputs[(i % ins)] = inputs[i];
			}
		}
	}

	inline void playback_outputs_stereo(double& lsample, double& rsample) {
		lsample = 0;
		rsample = 0;
		const size_t outs = plugin.info.output_channels;
		if (outs > 1) {
			for (size_t i = 0; i < outs; ++i) {
				if (i % 2 == 0) {
					lsample += outputs[i];
				}
				else {
					rsample += outputs[i];
				}
				outputs[i] = 0;
			}
		}
		else if (outs == 1) {
			lsample = outputs[0];
			rsample = outputs[0];
			outputs[0] = 0;
		}
	}

};

class PluginManager {
private:
	std::map<std::string, Plugin*> plugins;

public:

	std::vector<Plugin*> get_plugins() {
		std::vector<Plugin*> plugins;
		for (auto plugin : this->plugins) {
			plugins.push_back(plugin.second);
		}
		return plugins;
	}

	std::vector<Plugin*> get_plugins(PluginType type) {
		std::vector<Plugin*> plugins;
		for (auto plugin : this->plugins) {
			if (plugin.second->info.type == type) {
				plugins.push_back(plugin.second);
			}
		}
		return plugins;
	}

	std::vector<PluginInfo> get_infos() {
		std::vector<PluginInfo> infos;
		for (auto plugin : plugins) {
			infos.push_back(plugin.second->info);
		}
		return infos;
	}

	void add_plugin(Plugin* plugin) {
		if (get_plugin(plugin->info.identifier_name)) {
			throw "Conflicting plugin identifier! Plugin with name " + plugin->info.identifier_name + "already exists!";
		}
		else {
			plugins[plugin->info.identifier_name] = plugin;
		}
	}

	Plugin* get_plugin(std::string identifier) {
		if (plugins.find(identifier) == plugins.end()) {
			return nullptr;
		}
		else {
			return plugins[identifier];
		}
	}

	~PluginManager() {
		for (auto plugin : plugins) {
			delete plugin.second;
		}
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
		tree.put("plugin", program ? program->get_plugin_name() : "");
		if (program) {
			tree.put_child("data", program->save());
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

	void set_plugin_locked(Plugin* plugin, SpinLock& lock) {
		PluginInstance* inst = nullptr;
		if (plugin) {
			inst = plugin->create(host);
		}
		lock.lock();
		PluginInstance* old = this->plugin;
		this->plugin = inst;
		lock.unlock();

		delete old;
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
