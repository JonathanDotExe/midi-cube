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
#include "control.h"
#include "../dsp/envelope.h"
#include "../data/binding.h"
#include "../util/util.h"
#include "../util/clipboard.h"
#include "../gui/core.h"
#include "ui.h"
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

	//Pointer should always stay the same
	virtual const KeyboardEnvironment& get_environment() = 0;

	//Pointer should always stay the same
	virtual const Metronome& get_metronome() = 0;

	//Pointer should always stay the same
	virtual SpinLock& get_lock() = 0;

	//Pointer should always stay the same
	virtual MidiBindingHandler* get_binding_handler() = 0;

	virtual void recieve_midi(const MidiMessage& message, const SampleInfo& info, void* source) = 0;

	virtual Plugin* get_plugin(std::string identifier) = 0;

	virtual void notify_property_update(void* source, void* prop) = 0;

	virtual int get_transpose() = 0;

	//Pointer should always stay the same
	virtual const MidiControls& get_controls() = 0;

	virtual MasterPluginHost& get_master_host() = 0;

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

class PluginInstance : public ControlHost {

protected:

	void send_midi(const MidiMessage& msg, const SampleInfo& info) {
		host.recieve_midi(msg, info, this);
	}

public:
	double* inputs;
	double* outputs;
	PluginHost& host;
	Plugin& plugin;
	const MidiControls& controls; //Cache pointer
	const KeyboardEnvironment& host_environment; //Cache pointer
	const Metronome& host_metronome; //Cache pointer

	//Must be thread safe
	PluginInstance(PluginHost& h, Plugin& p) : host(h), plugin(p), controls(h.get_controls()), host_environment(h.get_environment()), host_metronome(h.get_metronome()) {
		inputs = new double[p.info.input_channels]();
		outputs = new double[p.info.output_channels]();
	}


	//In Audio Thread
	virtual void init() {

	}

	virtual void process(const SampleInfo& info) = 0;

	virtual void recieve_midi(const MidiMessage& message, const SampleInfo& info) = 0;

	virtual void apply_program(PluginProgram* prog) = 0;

	virtual void save_program(PluginProgram** prog) = 0;

	virtual bool keep_active() {
		return false;
	}

	const virtual MidiControls& get_controls() {
		return controls;
	}

	virtual Menu* create_menu() {
		return nullptr;
	}

	//In Audio Thread
	virtual void cleanup() {

	}

	//Must be thread safe
	virtual ~PluginInstance() {
		delete inputs;
		delete outputs;
	}

	inline SpinLock& get_lock() const {
		return host.get_lock();
	}

	inline void notify_property_update(void* prop) {
		host.notify_property_update(this, prop);
	}

	inline void take_input_mono(double sample) {
		const size_t ins = plugin.info.input_channels;
		switch (ins) {
		case 0:
			break;
		default:
			memset(inputs + 1, 0, (ins - 1) * sizeof(inputs[0]));
			/* no break */
		case 1:
			inputs[0] = sample;
			break;
		}
	}

	inline void take_input_stereo(double lsample, double rsample) {
		const size_t ins = plugin.info.input_channels;
		switch (ins) {
		case 0:
			break;
		case 1:
			inputs[0] = (rsample + lsample)/2;
			break;
		default:
			memset(inputs + 2, 0, (ins - 2)* sizeof(inputs[0]));
			/* no break */
		case 2:
			inputs[0] = lsample;
			inputs[1] = rsample;
			break;
		}
	}

	inline void take_input_stereo_and_inputs(double lsample, double rsample, double* inputs, const size_t input_count) {
		const size_t ins = plugin.info.input_channels;
		unsigned long int mic_inputs;
		switch (ins) {
		case 0:
			break;
		case 1:
			this->inputs[0] = (rsample + lsample)/2;
			break;
		default:
			mic_inputs = std::min((unsigned long int) ins - 2, (unsigned long int) input_count);
			memset(this->inputs + 2 + mic_inputs, 0, (ins - 2 - mic_inputs)* sizeof(inputs[0]));
			memcpy(this->inputs + 2, inputs, mic_inputs * sizeof(inputs[0]));
			/* no break */
		case 2:
			this->inputs[0] = lsample;
			this->inputs[1] = rsample;
			break;
		}
	}

	inline void take_inputs(double* inputs, const size_t input_count) {
		unsigned long int count = std::min((unsigned long int) input_count, (unsigned long int) plugin.info.input_channels);
		memcpy(this->inputs, inputs, count * sizeof(inputs[0]));
	}

	inline void playback_outputs_stereo(double& lsample, double& rsample) {
		const size_t outs = plugin.info.output_channels;
		switch (outs) {
		case 0:
			break;
		case 1:
			lsample = outputs[0];
			rsample = outputs[0];
			break;
		default:
			lsample = outputs[0];
			rsample = outputs[1];
			break;
		}
		memset(outputs, 0, sizeof(outputs[0]) * outs);
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

class PluginSlotProgram : public Copyable  {
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
			if (tree.get_child_optional("preset")) {
				program->load(tree.get_child("preset"));
			}
		}
	}

	pt::ptree save() {
		pt::ptree tree;
		tree.put("plugin", program ? program->get_plugin_name() : "");
		if (program) {
			tree.put_child("preset", program->save());
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

	void copy_plugin(Clipboard& clipboard);

	bool paste_plugin(Clipboard& clipboard, PluginManager& mgr);

	inline PluginInstance* get_plugin() const {
		return plugin;
	}

	void set_plugin(Plugin* plugin) {
		if (this->plugin) {
			this->plugin->cleanup();
			delete this->plugin;
			this->plugin = nullptr;
		}
		if (plugin) {
			this->plugin = plugin->create(host);
			this->plugin->init();
		}
	}

	void set_plugin_locked(Plugin* plugin, SpinLock& lock) {
		PluginInstance* inst = nullptr;
		if (plugin) {
			inst = plugin->create(host);
		}
		lock.lock();
		PluginInstance* old = this->plugin;
		if (old) {
			old->cleanup();
		}
		this->plugin = inst;
		if (this->plugin) {
			this->plugin->init();
		}
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

class MasterPluginHost : public ControlViewHost {

public:

	virtual PluginManager& get_plugin_manager() = 0;

	virtual MenuHandler& get_menu_handler() = 0;

	//Pointer should always stay the same
	virtual SpinLock& get_lock() = 0;

	//Pointer should always stay the same
	virtual MidiBindingHandler* get_binding_handler() = 0;

	virtual ~MasterPluginHost() {

	}

};



#endif
