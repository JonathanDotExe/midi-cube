/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_DELAY_H_
#define MIDICUBE_EFFECT_DELAY_H_

#include "../framework/core/audio.h"
#include "../framework/dsp/oscilator.h"
#include "../framework/core/plugins/effect.h"

#define DELAY_IDENTIFIER "midicube_delay"

/*
	cc.register_binding(new TemplateControlBinding<bool>("on", preset.on, false, true));
	cc.register_binding(new TemplateControlBinding<double>("mix", preset.mix, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("left_delay", preset.left_delay, 0, 5));
	cc.register_binding(new TemplateControlBinding<double>("left_init_delay_offset", preset.left_init_delay_offset, -5, 5));
	cc.register_binding(new TemplateControlBinding<double>("left_feedback", preset.left_feedback, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("right_delay", preset.right_delay, 0, 5));
	cc.register_binding(new TemplateControlBinding<double>("right_init_delay_offset", preset.right_init_delay_offset, -5, 5));
	cc.register_binding(new TemplateControlBinding<double>("right_feedback", preset.right_feedback, 0, 1));
	cc.register_binding(new TemplateControlBinding<bool>("stereo", preset.stereo, false, true));
 */

struct DelayPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> left_init_delay_offset{0.0, -5, 5};
	BindableTemplateValue<double> left_delay{0.1, 0, 5};
	BindableTemplateValue<double> left_feedback{0.2, 0, 1};
	BindableTemplateValue<double> right_init_delay_offset{0.0, -5, 5};
	BindableTemplateValue<double> right_delay{0.1, 0, 5};
	BindableTemplateValue<double> right_feedback{0.2, 0, 1};
	BindableBooleanValue stereo = true;
	BindableTemplateValue<double> mix{0.5, 0, 1};
};


class DelayProgram : public PluginProgram {
public:
	DelayPreset preset;

	virtual std::string get_plugin_name();
	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~DelayProgram() {

	}
};

class DelayEffect : public Effect {
private:
	AnalogOscilator osc;
	DelayBuffer ldelay;
	DelayBuffer rdelay;

public:
	DelayPreset preset;

	DelayEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	~DelayEffect();
};

class DelayPlugin : public EffectPlugin<DelayEffect, DelayProgram> {
public:
	DelayPlugin() : EffectPlugin({
		"Delay",
		DELAY_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_DELAY_H_ */
