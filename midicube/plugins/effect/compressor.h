/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_COMPRESSOR_H_
#define MIDICUBE_EFFECT_COMPRESSOR_H_

#include "../../framework/core/audio.h"
#include "../../framework/dsp/oscilator.h"
#include "../../framework/core/plugins/effect.h"

#define COMPRESSOR_IDENTIFIER "midicube_compressor"

struct CompressorPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> threshold{-12, -50, 0};
	BindableTemplateValue<double> ratio{4, 1, 15};
	BindableTemplateValue<double> attack{0.02, 0, 1};
	BindableTemplateValue<double> release{0.1, 0, 1};
	BindableTemplateValue<double> makeup_gain{3, 0, 5};
};


class CompressorProgram : public PluginProgram {
public:
	CompressorPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();


	virtual ~CompressorProgram() {

	}
};

class CompressorEffect : public Effect {
private:
	EnvelopeFollower lenv;
	EnvelopeFollower renv;
	PortamendoBuffer lvol{1};
	PortamendoBuffer rvol{1};

public:
	CompressorPreset preset;

	CompressorEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	Menu* create_menu();
	~CompressorEffect();
};

class CompressorPlugin : public EffectPlugin<CompressorEffect, CompressorProgram> {
public:
	CompressorPlugin() : EffectPlugin({
		"Compressor",
		COMPRESSOR_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_COMPRESSOR_H_ */
