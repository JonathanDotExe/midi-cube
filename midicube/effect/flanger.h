/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_FLANGER_H_
#define MIDICUBE_EFFECT_FLANGER_H_

#include "../audio.h"
#include "../oscilator.h"
#include "effect.h"

struct FlangerPreset {
	bool on = true;
	double vibrato_rate = 2;
	double vibrato_depth = 0.5;
	double mix = 0.5;

	double delay = 0.05;
	double feedback = 0.0;
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class FlangerProgram : public EffectProgram {
public:
	FlangerPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~FlangerProgram() {

	}
};

class FlangerEffect : public Effect {
private:
	AnalogOscilator osc;
	DelayBuffer ldelay;
	DelayBuffer rdelay;

public:
	FlangerPreset preset;

	FlangerEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~FlangerEffect();
};

#endif /* MIDICUBE_EFFECT_FLANGER_H_ */
