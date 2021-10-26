/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_BITCRUSHER_H_
#define MIDICUBE_EFFECT_BITCRUSHER_H_

#include "../../framework/core/plugins/effect.h"
#include "../../framework/core/audio.h"

#define BIT_CRUSHER_IDENTIFIER "midicube_bit_crusher"

struct BitCrusherPreset {
	BindableBooleanValue on{true};
	unsigned int bits = 16;
};


class BitCrusherProgram : public PluginProgram {
public:
	BitCrusherPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~BitCrusherProgram() {

	}
};

class BitCrusherEffect : public Effect {
public:
	BitCrusherPreset preset;

	BitCrusherEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	ViewController* create_view();
	~BitCrusherEffect();
};

class BitCrusherPlugin : public EffectPlugin<BitCrusherEffect, BitCrusherProgram> {
public:
	BitCrusherPlugin() : EffectPlugin({
		"Bit Crusher",
		BIT_CRUSHER_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_BITCRUSHER_H_ */
