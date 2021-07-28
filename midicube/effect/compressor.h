/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_COMPRESSOR_H_
#define MIDICUBE_EFFECT_COMPRESSOR_H_

#include "../audio.h"
#include "../oscilator.h"
#include "effect.h"

struct CompressorPreset {
	BindableBooleanValue on = true;
	BindableTemplateValue<double> threshold{-20, -50, 0};
	BindableTemplateValue<double> ratio{4, 1, 15};
	BindableTemplateValue<double> attack{0.1, 0, 1};
	BindableTemplateValue<double> release{0.1, 0, 1};
	BindableTemplateValue<double> makeup_gain{1, 0, 5};
};


class CompressorProgram : public EffectProgram {
public:
	CompressorPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~CompressorProgram() {

	}
};

class CompressorEffect : public Effect {
private:
	EnvelopeFollower lenv;
	EnvelopeFollower renv;
	PortamendoBuffer lvol{1, 0};
	PortamendoBuffer rvol{1, 0};

public:
	CompressorPreset preset;

	CompressorEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~CompressorEffect();
};

#endif /* MIDICUBE_EFFECT_COMPRESSOR_H_ */
