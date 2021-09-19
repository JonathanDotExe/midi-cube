/*
 * Equalizer.h
 *
 *  Created on: May 27, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_EQUALIZER_H_
#define MIDICUBE_EFFECT_EQUALIZER_H_

#include "../framework/core/audio.h"
#include "../framework/dsp/oscilator.h"
#include "effect.h"

struct EqualizerPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> low_freq{100, 20, 400};
	BindableTemplateValue<double> low_gain{0, -1, 5};
	BindableTemplateValue<double> low_mid_freq{400, 100, 1000};
	BindableTemplateValue<double> low_mid_gain{0, -1, 5};
	BindableTemplateValue<double> mid_freq{1000, 200, 8000};
	BindableTemplateValue<double> mid_gain{0, -1, 5};
	BindableTemplateValue<double> high_freq{4000, 1000, 20000};
	BindableTemplateValue<double> high_gain{0, -1, 5};
};


class EqualizerProgram : public EffectProgram {
public:
	EqualizerPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~EqualizerProgram() {

	}
};

class EqualizerEffect : public Effect {
private:
	Filter llowfilter;
	Filter rlowfilter;
	Filter llow_midfilter;
	Filter rlow_midfilter;
	Filter lmidfilter;
	Filter rmidfilter;
	Filter lhighfilter;
	Filter rhighfilter;

public:
	EqualizerPreset preset;

	EqualizerEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~EqualizerEffect();
};

#endif /* MIDICUBE_EFFECT_EQUALIZER_H_ */
