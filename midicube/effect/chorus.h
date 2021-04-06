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
	bool on = false;
	double vibrato_rate = 6;
	double vibrato_depth = 0;
	double mix = 0.5;

	double delay = 0.015;
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
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
