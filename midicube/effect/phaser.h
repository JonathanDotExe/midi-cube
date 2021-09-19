/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_PHASER_H_
#define MIDICUBE_EFFECT_PHASER_H_

#include "../framework/core/audio.h"
#include "../framework/dsp/oscilator.h"
#include "effect.h"

#define PHASER_ALLPASS_AMOUNT 4

struct PhaserPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> center_cutoff{0.5, 0, 1};
	BindableTemplateValue<double> lfo_depth{0.5, 0, 1};
	BindableTemplateValue<double> lfo_rate{1, 0, 8};
	BindableTemplateValue<double> mix{0.5, 0, 1};
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class PhaserProgram : public EffectProgram {
public:
	PhaserPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~PhaserProgram() {

	}
};

class PhaserEffect : public Effect {
private:
	AnalogOscilator osc;
	std::array<Filter, 4> lfilter;
	std::array<Filter, 4> rfilter;

public:
	PhaserPreset preset;

	PhaserEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~PhaserEffect();
};

#endif /* MIDICUBE_EFFECT_PHASER_H_ */
