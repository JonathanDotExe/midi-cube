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
#include "../synthesis.h"

#define ROTARY_CUTOFF 800

#define ROTARY_HORN_SLOW_FREQUENCY 0.83333333333333
#define ROTARY_HORN_FAST_FREQUENCY 6.66666666666667
#define ROTARY_BASS_SLOW_FREQUENCY 0.66666666666667
#define ROTARY_BASS_FAST_FREQUENCY 5.66666666666667

#define ROTARY_HORN_SLOW_RAMP 1.6
#define ROTARY_HORN_FAST_RAMP 1.0
#define ROTARY_BASS_SLOW_RAMP 5.5
#define ROTARY_BASS_FAST_RAMP 5.5

struct RotarySpeakerPreset {
	bool on = false;
	bool fast = false;

	double stereo_mix{0.5};
	bool type{false};
	double max_delay{0.001};

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
	Filter filter;
	FilterData filter_data;

	DelayBuffer left_delay;
	DelayBuffer right_delay;
	bool curr_rotary_fast = 0;
	PortamendoBuffer horn_speed{ROTARY_HORN_SLOW_FREQUENCY, ROTARY_HORN_SLOW_RAMP};
	PortamendoBuffer bass_speed{ROTARY_BASS_SLOW_FREQUENCY, ROTARY_BASS_SLOW_RAMP};
	double horn_rotation = 0;
	double bass_rotation = 0;
public:
	RotarySpeakerEffect();
	void apply(double& lsample, double& rsample, RotarySpeakerPreset& preset, SampleInfo& info);
	virtual ~RotarySpeakerEffect();
};

#endif /* MIDICUBE_EFFECT_ROTARY_SPEAKER_H_ */
