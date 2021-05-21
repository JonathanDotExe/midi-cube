/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_WAH_WAH_H_
#define MIDICUBE_EFFECT_WAH_WAH_H_

#include "../audio.h"
#include "../oscilator.h"
#include "effect.h"


struct WahWahPreset {
	bool on = true;
	double cutoff = 0.2;
	double resonance = 0.5;
	double amount = 0.5;
	double mix = 1;

	bool auto_wah = true;
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
