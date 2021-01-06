/*
 * RotarySpeaker.h
 *
 *  Created on: Jan 6, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_ROTARY_SPEAKER_H_
#define MIDICUBE_EFFECT_ROTARY_SPEAKER_H_

#include "../audio.h"
#include "../filter.h"
#include "../util.h"

#define ROTARY_CUTOFF 800

struct RotarySpeakerPreset {
	bool on = false;
	bool fast = false;
	unsigned int on_cc{22};
	unsigned int speed_cc{23};

	double stereo_mix{0.5};
	double gain{1.5};
	bool type{true};
	double max_delay{0.0005};

	double horn_slow_frequency = 0.83333333333333;
	double horn_fast_frequency = 6.66666666666667;
	double bass_slow_frequency = 0.66666666666667;
	double bass_fast_frequency = 5.66666666666667;

	double horn_slow_ramp = 1.6;
	double horn_fast_ramp = 1.0;
	double bass_slow_ramp = 5.5;
	double bass_fast_ramp = 5.5;
};

class RotarySpeakerEffect {
private:

public:
	RotarySpeakerEffect();
	void apply(double& lsample, double& rsample, RotarySpeakerPreset& preset, SampleInfo& info);
	virtual ~RotarySpeakerEffect();
};

#endif /* MIDICUBE_EFFECT_ROTARY_SPEAKER_H_ */
