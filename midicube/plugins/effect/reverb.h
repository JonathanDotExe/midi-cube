/*
 * reverb.h
 *
 *  Created on: Apr 19, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_REVERB_H_
#define MIDICUBE_EFFECT_REVERB_H_

#include "../../framework/core/plugins/effect.h"
#include "../../framework/core/audio.h"
#include "../../framework/dsp/synthesis.h"


#define REVERB_IDENTIFIER "midicube_schroeder_reverb"


struct ReverbPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> delay{0.2, 0, 2};
	BindableTemplateValue<double> decay{0.7, 0, 1};
	BindableTemplateValue<double> mix{0.5, 0, 1};

	BindableTemplateValue<double> tone{0.35, 0, 1};
	BindableTemplateValue<double> resonance{0, 0, 1};

	BindableTemplateValue<double> stereo{0, -1, 1};
};

class ReverbProgram : public PluginProgram {
public:
	ReverbPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~ReverbProgram() {

	}
};

class ReverbEffect : public Effect {
private:


public:
	ReverbPreset preset;

	ReverbEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	Menu* create_menu();
	~ReverbEffect();
};

class ReverbPlugin : public EffectPlugin<ReverbEffect, ReverbProgram> {
public:
	ReverbPlugin() : EffectPlugin({
		"Reverb",
		REVERB_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_REVERB_H_ */
