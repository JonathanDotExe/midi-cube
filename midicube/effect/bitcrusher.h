/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_BITCRUSHER_H_
#define MIDICUBE_EFFECT_BITCRUSHER_H_

#include "../audio.h"
#include "effect.h"

struct BitCrusherPreset {
	bool on = true;
	unsigned int bits = 16;
};


class BitCrusherProgram : public EffectProgram {
public:
	BitCrusherPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~BitCrusherProgram() {

	}
};

class BitCrusherEffect : public Effect {
public:
	BitCrusherPreset preset;

	BitCrusherEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	EffectProgram* create_program();
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~BitCrusherEffect();
};

#endif /* MIDICUBE_EFFECT_BITCRUSHER_H_ */
