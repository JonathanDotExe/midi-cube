/*
 * Equalizer.h
 *
 *  Created on: May 27, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_EQUALIZER_H_
#define MIDICUBE_EFFECT_EQUALIZER_H_

#include "../audio.h"
#include "../oscilator.h"
#include "effect.h"

struct EqualizerPreset {
	bool on = true;
	double low_freq = 100;
	double low_gain = 0;
	double low_mid_freq = 400;
	double low_mid_gain = 0;
	double mid_freq = 1000;
	double mid_gain = 0;
	double high_freq = 4000;
	double high_gain = 0;
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
