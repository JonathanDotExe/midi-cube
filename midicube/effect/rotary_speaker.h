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
#include "effect.h"

#define ROTARY_CUTOFF 800


#define ROTARY_HORN_SLOW_FREQUENCY 0.85
#define ROTARY_HORN_FAST_FREQUENCY 6.65
#define ROTARY_BASS_SLOW_FREQUENCY 0.65
#define ROTARY_BASS_FAST_FREQUENCY 5.65

/*#define ROTARY_HORN_SLOW_FREQUENCY 2
#define ROTARY_HORN_FAST_FREQUENCY 8.3
#define ROTARY_BASS_SLOW_FREQUENCY 0.25
#define ROTARY_BASS_FAST_FREQUENCY 5*/

#define ROTARY_HORN_SLOW_RAMP 1.6
#define ROTARY_HORN_FAST_RAMP 1.0
#define ROTARY_BASS_SLOW_RAMP 5.5
#define ROTARY_BASS_FAST_RAMP 5.5

struct RotarySpeakerPreset {
	bool on = true;
	bool fast = false;

	double stereo_mix{0.7};
	bool type{false};
	double max_delay{0.0005};
	double min_amplitude = 0.5;
	double mix = 1;

	double horn_slow_frequency = ROTARY_HORN_SLOW_FREQUENCY;
	double horn_fast_frequency = ROTARY_HORN_FAST_FREQUENCY;
	double bass_slow_frequency = ROTARY_BASS_SLOW_FREQUENCY;
	double bass_fast_frequency = ROTARY_BASS_FAST_FREQUENCY;

	double horn_slow_ramp = ROTARY_HORN_SLOW_RAMP;
	double horn_fast_ramp = ROTARY_HORN_FAST_RAMP;
	double bass_slow_ramp = ROTARY_BASS_SLOW_RAMP;
	double bass_fast_ramp = ROTARY_BASS_FAST_RAMP;
};


class RotarySpeakerProgram : public EffectProgram {
public:
	RotarySpeakerPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~RotarySpeakerProgram() {

	}
};

class RotarySpeakerEffect : public Effect {
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
	RotarySpeakerPreset preset;

	RotarySpeakerEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~RotarySpeakerEffect();
};

#endif /* MIDICUBE_EFFECT_ROTARY_SPEAKER_H_ */
