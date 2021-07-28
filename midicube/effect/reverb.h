/*
 * reverb.h
 *
 *  Created on: Apr 19, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_REVERB_H_
#define MIDICUBE_EFFECT_REVERB_H_

#include "effect.h"

#include "../synthesis.h"

#define REVERB_COMB_FILTERS 4
#define REVERB_ALLPASS_FILTERS 2

/*
	cc.register_binding(new TemplateControlBinding<bool>("on", preset.on, false, true));
	cc.register_binding(new TemplateControlBinding<double>("delay", preset.delay, 0, 2));
	cc.register_binding(new TemplateControlBinding<double>("decay", preset.decay, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("mix", preset.mix, 0, 1));

	cc.register_binding(new TemplateControlBinding<double>("tone", preset.tone, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("resonance", preset.resonance, 0, 1));
	cc.register_binding(new TemplateControlBinding<double>("stereo", preset.stereo, -1, 1));
 */

struct ReverbPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> delay{0.2, 0, 2};
	BindableTemplateValue<double> decay{0.7, 0, 1};
	BindableTemplateValue<double> mix{0.5, 0, 1};

	BindableTemplateValue<double> tone{0.35, 0, 1};
	BindableTemplateValue<double> resonance{0, 0, 1};

	BindableTemplateValue<double> stereo{0, -1, 1};
};

class CombFilter {
private:
	DelayBuffer delay;

public:

	double process(double in, double gain, unsigned int delay);
};

class AllPassFilter {
private:
	DelayBuffer indelay;
	DelayBuffer delay;

public:

	double process(double in, double gain, unsigned int delay);
};

class ReverbProgram : public EffectProgram {
public:
	ReverbPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~ReverbProgram() {

	}
};

class ReverbEffect : public Effect {
private:
	std::array<CombFilter, 4> lcomb_filters;
	std::array<CombFilter, 4> rcomb_filters;
	std::array<AllPassFilter, 2> lallpass_filters;
	std::array<AllPassFilter, 2> rallpass_filters;

	Filter lfilter;
	Filter rfilter;

	std::array<double, REVERB_COMB_FILTERS> comb_delay_mul = {1, 1.13287, 0.6812463, 0.622141};
	std::array<double, REVERB_COMB_FILTERS> comb_decay_mul = {0.57962, 0.55271987, 0.981233, 1};

	double allpass_delay = 0.09;
	double allpass_decay = 0.015;

public:
	ReverbPreset preset;

	ReverbEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~ReverbEffect();
};

#endif /* MIDICUBE_EFFECT_REVERB_H_ */
