/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_CHORUS_H_
#define MIDICUBE_EFFECT_CHORUS_H_

#include "../audio.h"
#include "../oscilator.h"
#include "effect.h"

struct ChorusPreset {
	bool on = true;
	double vibrato_rate = 2;
	double vibrato_depth = 0.5;
	double mix = 0.5;

	double delay = 0.015;
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class ChorusProgram : public EffectProgram {
public:
	ChorusPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~ChorusProgram() {

	}
};

class ChorusEffect : public Effect {
private:
	AnalogOscilator osc;
	DelayBuffer ldelay;
	DelayBuffer rdelay;

public:
	ChorusPreset preset;

	ChorusEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	~ChorusEffect();
};

#endif /* MIDICUBE_EFFECT_CHORUS_H_ */
