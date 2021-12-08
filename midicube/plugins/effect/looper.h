/*
 * looper.h
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_LOOPER_H_
#define MIDICUBE_SOUNDENGINE_LOOPER_H_

#include <array>
#include <atomic>
#include "../../framework/core/audio.h"
#include "../../framework/core/metronome.h"
#include "../../framework/core/plugins/effect.h"

#define LOOPER_IDENTIFIER "midicube_audio_flanger"

//16 MB
#define LOOPER_CHANNELS 8
#define LOOPER_BUFFER_SIZE 1048576

class LooperProgram : public PluginProgram {
public:
	void load(boost::property_tree::ptree tree);
	std::string get_plugin_name();
	boost::property_tree::ptree save();
};

struct LooperPreset {
	unsigned int bars{4};
};

class LooperChannel {
private:
	std::array<double, LOOPER_BUFFER_SIZE> lbuffer = {};
	std::array<double, LOOPER_BUFFER_SIZE> rbuffer = {};


public:
	LooperPreset preset;
	bool play{true};
	bool reset{false};	//true will invoke a reset next sample

	void apply(double& lout, double& rout, const Metronome& metronome, const SampleInfo& info);

	void record(double lin, double rin, const Metronome& metronome, const SampleInfo& info);
};

class Looper : public Effect {
public:
	std::array<LooperChannel, LOOPER_CHANNELS> channels;
	bool active = false;
	ssize_t record_channel = -1;
	ssize_t solo_channel = -1;

	Looper(PluginHost& h, Plugin& p) : Effect(h, p) {

	}

	void apply_program(PluginProgram *prog);
	void process(const SampleInfo &info);
	void save_program(PluginProgram **prog);
	ViewController* create_menu();
	void recieve_midi(const MidiMessage &message, const SampleInfo &info);

};

class LooperPlugin : public EffectPlugin<Looper, LooperProgram> {
public:
	LooperPlugin() : EffectPlugin({
		"Looper",
		LOOPER_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_SOUNDENGINE_LOOPER_H_ */
