/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_BITCRUSHER_H_
#define MIDICUBE_EFFECT_BITCRUSHER_H_

#include "../audio.h"

struct BitCrusherPreset {
	bool on = false;
	unsigned int bits = 16;
};

class BitCrusherEffect {
public:
	BitCrusherEffect();
	void apply(double& lsample, double& rsample, BitCrusherPreset& preset, SampleInfo& info);
	virtual ~BitCrusherEffect();
};

#endif /* MIDICUBE_EFFECT_BITCRUSHER_H_ */
