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
#include "effect.h"

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


class DelayProgram : public EffectProgram {
public:
	DelayPreset preset;

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

	DelayEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~DelayEffect();
};

#endif /* MIDICUBE_EFFECT_DELAY_H_ */
