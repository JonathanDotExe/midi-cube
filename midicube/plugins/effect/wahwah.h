/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_WAH_WAH_H_
#define MIDICUBE_EFFECT_WAH_WAH_H_

#include "../../framework/core/audio.h"
#include "../../framework/dsp/oscilator.h"
#include "../../framework/core/plugins/effect.h"

#define WAH_WAH_IDENTIFIER "midicube_wah_wah"

struct WahWahPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> cutoff{0.2, 0, 1};
	BindableTemplateValue<double> resonance{0.5, 0, 1};
	BindableTemplateValue<double> amount{0.5, 0, 10};
	BindableTemplateValue<double> mix{1, 0, 1};

	BindableBooleanValue auto_wah = true;
};


class WahWahProgram : public PluginProgram {
public:
	WahWahPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~WahWahProgram() {

	}
};

class WahWahEffect : public Effect {
private:
	Filter lfilter;
	Filter rfilter;
	EnvelopeFollower lenv;
	EnvelopeFollower renv;

public:
	WahWahPreset preset;

	WahWahEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	ViewController* create_view();
	~WahWahEffect();
};

class WahWahPlugin : public EffectPlugin<WahWahEffect, WahWahProgram> {
public:
	WahWahPlugin() : EffectPlugin({
		"Wah-Wah",
		WAH_WAH_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_WAH_WAH_H_ */
