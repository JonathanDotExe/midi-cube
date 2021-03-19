/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_BITCRUSHER_H_
#define MIDICUBE_EFFECT_BITCRUSHER_H_

#include "../audio.h"
#include "../oscilator.h"

struct ChorusPreset {
	bool on = false;
	double vibrato_rate = 6;
	double vibrato_depth = 0;
	double mix = 0.5;

	double delay = 0.015;
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
};

class ChorusEffect {
private:
	AnalogOscilator osc;
	DelayBuffer ldelay;
	DelayBuffer rdelay;

public:
	ChorusEffect();
	void apply(double& lsample, double& rsample, ChorusPreset& preset, SampleInfo& info);
	virtual ~ChorusEffect();
};

#endif /* MIDICUBE_EFFECT_BITCRUSHER_H_ */
