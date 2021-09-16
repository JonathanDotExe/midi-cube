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

struct MidiCubePluginInfo {
	std::string name = "Plugin";
	unsigned int input_channels = 0;
	unsigned int output_channels = 0;
	bool input_midi = false;
	bool output_midi = false;
};


class Plugin {



};

class PluginInstance {

};
