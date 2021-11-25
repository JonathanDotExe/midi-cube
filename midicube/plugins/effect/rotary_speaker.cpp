/*
 * RotarySpeaker.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: jojo
 */

#include "rotary_speaker.h"
#include "../view/EffectView.h"

static inline double sound_delay(double rotation, double max_delay, unsigned int sample_rate) {
	return (1 + rotation) * max_delay * 0.5 * sample_rate;
}

RotarySpeakerEffect::RotarySpeakerEffect(PluginHost& h, Plugin& p) : Effect(h, p) {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.fast);

	cc.init(get_host().get_binding_handler(), this);

	filter_data.type = FilterType::LP_24;
	filter_data.cutoff = 800;
}

void RotarySpeakerEffect::process(const SampleInfo &info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		//Sum samples up
		double sample = (outputs[0] + outputs[1]) / 2.0;

		//Drive
		sample = apply_distortion(sample, preset.drive, DistortionType::CUBIC_DISTORTION);

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
		left_delay.add_isample(horn_sample * ((1 - preset.min_amplitude) + (0.5 + horn_rot * 0.5) * preset.min_amplitude), lhorn_delay);
		left_delay.add_isample(bass_sample * ((1 - preset.min_amplitude) + (0.5 + bass_rot * 0.5) * preset.min_amplitude), lbass_delay);
		right_delay.add_isample(horn_sample * ((1 - preset.min_amplitude) + (0.5 - horn_rot * 0.5) * preset.min_amplitude), rhorn_delay);
		right_delay.add_isample(bass_sample * ((1 - preset.min_amplitude) + (0.5 - bass_rot * 0.5) * preset.min_amplitude), rbass_delay);

		//Play delay
		double l = left_delay.process();
		double r = right_delay.process();
		double ls = l + r * preset.stereo_mix;
		double rs = r + l * preset.stereo_mix;
		ls *= 2.0/(1 + preset.stereo_mix);
		rs *= 2.0/(1 + preset.stereo_mix);

		//Mix
		outputs[0] *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		outputs[1] *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		outputs[0] += ls * fmin(0.5, preset.mix) * 2;
		outputs[1] += rs * fmin(0.5, preset.mix) * 2;
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
	if (curr_rotary_stop != preset.stop) {

	}
}

ViewController* RotarySpeakerEffect::create_view() {
	return new EffectView(this);
}

RotarySpeakerEffect::~RotarySpeakerEffect() {

}

void RotarySpeakerProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.fast.load(tree, "fast", false);

	preset.stereo_mix = tree.get<double>("stereo_mix", 0.7);
	preset.type = tree.get<bool>("type", false);
	preset.max_delay = tree.get<double>("max_delay", 0.0005);
	preset.min_amplitude = tree.get<double>("min_amplitude", 0.5);
	preset.mix = tree.get<double>("mix", 1);

	preset.horn_slow_frequency = tree.get<double>("horn_slow_frequency", ROTARY_HORN_SLOW_FREQUENCY);
	preset.horn_fast_frequency = tree.get<double>("horn_fast_frequency", ROTARY_HORN_FAST_FREQUENCY);
	preset.bass_slow_frequency = tree.get<double>("bass_slow_frequency", ROTARY_BASS_SLOW_FREQUENCY);
	preset.bass_fast_frequency = tree.get<double>("bass_fast_frequency", ROTARY_BASS_FAST_FREQUENCY);

	preset.horn_slow_ramp = tree.get<double>("horn_slow_ramp", ROTARY_HORN_SLOW_RAMP);
	preset.horn_fast_ramp = tree.get<double>("horn_fast_ramp", ROTARY_HORN_FAST_RAMP);
	preset.bass_slow_ramp = tree.get<double>("bass_slow_ramp", ROTARY_BASS_SLOW_RAMP);
	preset.bass_fast_ramp = tree.get<double>("bass_fast_ramp", ROTARY_BASS_FAST_RAMP);
}

boost::property_tree::ptree RotarySpeakerProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("fast", preset.fast.save());

	tree.put("stereo_mix", preset.stereo_mix);
	tree.put("type", preset.type);
	tree.put("max_delay", preset.max_delay);
	tree.put("min_amplitude", preset.min_amplitude);
	tree.put("mix", preset.mix);

	tree.put("horn_slow_frequency", preset.horn_slow_frequency);
	tree.put("horn_fast_frequency", preset.horn_fast_frequency);
	tree.put("bass_slow_frequency", preset.horn_slow_frequency);
	tree.put("bass_fast_frequency", preset.bass_fast_frequency);

	tree.put("horn_slow_ramp", preset.horn_slow_ramp);
	tree.put("horn_fast_ramp", preset.horn_fast_ramp);
	tree.put("bass_slow_ramp", preset.horn_slow_ramp);
	tree.put("bass_fast_ramp", preset.bass_fast_ramp);

	return tree;
}

void RotarySpeakerEffect::save_program(PluginProgram **prog) {
	RotarySpeakerProgram* p = dynamic_cast<RotarySpeakerProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new RotarySpeakerProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void RotarySpeakerEffect::apply_program(PluginProgram *prog) {
	RotarySpeakerProgram* p = dynamic_cast<RotarySpeakerProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}

std::string RotarySpeakerProgram::get_plugin_name() {
	return ROTARY_SPEAKER_IDENTIFIER;
}
