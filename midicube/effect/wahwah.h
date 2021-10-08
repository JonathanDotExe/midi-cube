/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_WAH_WAH_H_
#define MIDICUBE_EFFECT_WAH_WAH_H_

#include "../framework/core/audio.h"
#include "../framework/dsp/oscilator.h"
#include "../framework/core/plugins/effect.h"

struct WahWahPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> cutoff{0.2, 0, 1};
	BindableTemplateValue<double> resonance{0.5, 0, 1};
	BindableTemplateValue<double> amount{0.5, 0, 10};
	BindableTemplateValue<double> mix{1, 0, 1};

	BindableBooleanValue auto_wah = true;
};


class WahWahProgram : public EffectProgram {
public:
	WahWahPreset preset;

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
	double pedal = 0;

	WahWahEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~WahWahEffect();
};

#endif /* MIDICUBE_EFFECT_WAH_WAH_H_ */
