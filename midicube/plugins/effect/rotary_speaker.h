/*
 * RotarySpeaker.h
 *
 *  Created on: Jan 6, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_ROTARY_SPEAKER_H_
#define MIDICUBE_EFFECT_ROTARY_SPEAKER_H_

#include "../../framework/core/audio.h"
#include "../../framework/dsp/filter.h"
#include "../../framework/dsp/distortion.h"
#include "../../framework/dsp/synthesis.h"
#include "../../framework/core/plugins/effect.h"

#define ROTARY_SPEAKER_IDENTIFIER "midicube_rotary_speaker"

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

enum RotaryState {
	ROTARY_STOP, ROTARY_SLOW, ROTARY_FAST
};

struct RotarySpeakerPreset {
	BindableBooleanValue on = true;
	BindableBooleanValue stop = false;
	BindableBooleanValue fast = false;
	BindableTemplateValue<double> drive{0, 0, 1};

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

	inline RotaryState state() {
		return fast ? RotaryState::ROTARY_FAST : (stop ? RotaryState::ROTARY_STOP : RotaryState::ROTARY_SLOW);
	}
};


class RotarySpeakerProgram : public PluginProgram {
public:
	RotarySpeakerPreset preset;

	virtual std::string get_plugin_name();
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
	RotaryState curr_rotary_state = RotaryState::ROTARY_STOP;
	PortamendoBuffer horn_speed{0, 0};
	PortamendoBuffer bass_speed{0, 0};
	double horn_rotation = 0;
	double bass_rotation = 0;
public:
	RotarySpeakerPreset preset;

	RotarySpeakerEffect(PluginHost& h, Plugin& p);
	void process(const SampleInfo& info);
	void save_program(PluginProgram **prog);
	void apply_program(PluginProgram *prog);
	Menu* create_menu();
	~RotarySpeakerEffect();
};

class RotarySpeakerPlugin : public EffectPlugin<RotarySpeakerEffect, RotarySpeakerProgram> {
public:
	RotarySpeakerPlugin() : EffectPlugin({
		"Rotary Speaker",
		ROTARY_SPEAKER_IDENTIFIER,
		PluginType::PLUGIN_TYPE_EFFECT,
		2,
		2,
		false,
		false
	}) {

	}
};

#endif /* MIDICUBE_EFFECT_ROTARY_SPEAKER_H_ */
