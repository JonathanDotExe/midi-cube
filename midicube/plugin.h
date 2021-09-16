/*
 * plugin.h
 *
 *  Created on: 16 Sep 2021
 *      Author: jojo
 */

#include <string>

enum class PluginType {
	SOUND_ENGINE, EFFECT, SEQUENCER, OTHER
};

struct PluginInfo {
	std::string name = "Plugin";
	unsigned int input_channels = 0;
	unsigned int output_channels = 0;
	bool input_midi = false;
	bool output_midi = false;
};

class PluginInstance;

class Plugin {

public:

	const PluginInfo info;

	Plugin(PluginInfo i) : info(i) {

	}

	virtual PluginInstance* create() = 0;

	virtual ~Plugin() {

	}

};

class PluginInstance {

private:
	const Plugin* plugin;

public:

	PluginInstance(Plugin* p) : plugin(p) {

	}

	Plugin* get_plugin() {
		return plugin;
	}

};
