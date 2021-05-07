/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_DELAY_H_
#define MIDICUBE_EFFECT_DELAY_H_

#include "../audio.h"
#include "../oscilator.h"
#include "effect.h"

struct DelayPreset {
	bool on = true;
	double left_init_delay_offset = 0.0;
	double left_delay = 0.1;
	double left_feedback = 0.2;
	double right_init_delay_offset = 0.0;
	double right_delay = 0.1;
	double right_feedback = 0.2;
	bool stereo = false;
	double mix = 0.5;
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
