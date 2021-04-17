/*
 * RotarySpeaker.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: jojo
 */

#include "rotary_speaker.h"

static inline double sound_delay(double rotation, double max_delay, unsigned int sample_rate) {
	return (1 + rotation) * max_delay * 0.5 * sample_rate;
}

RotarySpeakerEffect::RotarySpeakerEffect() {
	filter_data.type = FilterType::LP_24;
	filter_data.cutoff = 800;
}

void RotarySpeakerEffect::apply(double &lsample, double &rsample, SampleInfo &info) {
	if (preset.on) {
		//Sum samples up
		double sample = (lsample + rsample) / 2.0;
		//Filter
		double bass_sample = filter.apply(filter_data, sample, info.time_step);
		double horn_sample = sample - bass_sample;

		//Horn
		double horn_pitch_rot = preset.type ? sin(freq_to_radians(horn_rotation)) : cos(freq_to_radians(horn_rotation));
		double lhorn_delay = sound_delay(horn_pitch_rot, preset.max_delay, info.sample_rate);
		double rhorn_delay = sound_delay(-horn_pitch_rot, preset.max_delay, info.sample_rate);
		//Bass
		double bass_pitch_rot = preset.type ? sin(freq_to_radians(bass_rotation)) : cos(freq_to_radians(bass_rotation));
		double lbass_delay = sound_delay(bass_pitch_rot, preset.max_delay, info.sample_rate);
		double rbass_delay = sound_delay(-bass_pitch_rot, preset.max_delay, info.sample_rate);

		//Rotations
		double horn_rot = sin(freq_to_radians(horn_rotation));
		double bass_rot = sin(freq_to_radians(bass_rotation));

		//Process
		left_delay.add_isample(horn_sample * (0.5 + horn_rot * 0.5), lhorn_delay);
		left_delay.add_isample(bass_sample * (0.5 + bass_rot * 0.5), lbass_delay);
		right_delay.add_isample(horn_sample * (0.5 - horn_rot * 0.5), rhorn_delay);
		right_delay.add_isample(bass_sample * (0.5 - bass_rot * 0.5), rbass_delay);

		//Play delay
		double l = left_delay.process();
		double r = right_delay.process();
		lsample = l + r * preset.stereo_mix;
		rsample = r + l * preset.stereo_mix;
		lsample *= 2.0/(1 + preset.stereo_mix);
		rsample *= 2.0/(1 + preset.stereo_mix);
	}

	//Rotate speakers
	horn_rotation += horn_speed.get(info.time) * info.time_step;
	bass_rotation -= bass_speed.get(info.time) * info.time_step;

	//Switch speaker speed
	if (curr_rotary_fast != preset.fast) {
		curr_rotary_fast = preset.fast;
		if (curr_rotary_fast) {
			horn_speed.set(preset.horn_fast_frequency, info.time, preset.horn_fast_ramp);
			bass_speed.set(preset.bass_fast_frequency, info.time, preset.horn_slow_frequency);
		}
		else {
			horn_speed.set(preset.horn_slow_frequency, info.time, preset.horn_slow_ramp);
			bass_speed.set(preset.bass_slow_frequency, info.time, preset.bass_slow_ramp);
		}
	}
}


RotarySpeakerEffect::~RotarySpeakerEffect() {
	// TODO Auto-generated destructor stub
}

template<>
std::string get_effect_name<RotarySpeakerEffect>() {
	return "Rotary Speaker";
}

void RotarySpeakerProgram::load(boost::property_tree::ptree tree) {
	preset.on = tree.get<bool>("on", true);
	preset.fast = tree.get<bool>("fast", false);

	preset.stereo_mix = tree.get<double>("stereo_mix", 0.5);
	preset.type = tree.get<bool>("type", false);
}

boost::property_tree::ptree RotarySpeakerProgram::save() {
	boost::property_tree::ptree tree;
	tree.put("on", preset.on);
	tree.put("fast", preset.fast);

	tree.put("stereo_mix", preset.stereo_mix);
	tree.put("type", preset.type);

	return tree;
}

