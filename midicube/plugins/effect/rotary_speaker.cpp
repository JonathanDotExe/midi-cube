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

	lfilter_data.type = FilterType::LP_24;
	lfilter_data.cutoff = ROTARY_CUTOFF;
	hfilter_data.type = FilterType::HP_24;
	hfilter_data.cutoff = ROTARY_CUTOFF;
}

void RotarySpeakerEffect::process(const SampleInfo &info) {
	outputs[0] = inputs[0];
	outputs[1] = inputs[1];
	if (preset.on) {
		//Sum samples up
		double sample = (outputs[0] + outputs[1]) / 2.0;

		//Drive
		//sample = apply_distortion(sample, preset.drive, DistortionType::CUBIC_DISTORTION);

		//Filter
		double bass_sample = lfilter.apply(lfilter_data, sample, info.time_step);
		double horn_sample = sample - bass_sample /*hfilter.apply(hfilter_data, sample, info.time_step)*/;

		//Horn
		//TODO maybe apply sine at playback
		double horn_pitch_rot = preset.type ? sin(freq_to_radians(horn_rotation)) : cos(freq_to_radians(horn_rotation));
		double lhorn_delay = sound_delay(horn_pitch_rot, preset.max_delay, info.sample_rate);
		double rhorn_delay = sound_delay(-horn_pitch_rot, preset.max_delay, info.sample_rate);
		//Bass
		double bass_pitch_rot = preset.type ? sin(freq_to_radians(bass_rotation)) : cos(freq_to_radians(bass_rotation));
		double lbass_delay = sound_delay(bass_pitch_rot, preset.max_delay, info.sample_rate);
		double rbass_delay = lbass_delay /*sound_delay(-bass_pitch_rot, preset.max_delay, info.sample_rate)*/;

		//Rotations
		double horn_rot = sin(freq_to_radians(horn_rotation));
		double bass_rot = sin(freq_to_radians(bass_rotation));

		//Process
		left_delay.add_isample(horn_sample * ((1 - preset.min_amplitude) + (0.5 + horn_rot * 0.5) * preset.min_amplitude), lhorn_delay);
		double delayed_bass = bass_sample * ((1 - preset.min_amplitude) + (0.5 + bass_rot * 0.5) * preset.min_amplitude);
		left_delay.add_isample(delayed_bass, lbass_delay);
		right_delay.add_isample(horn_sample * ((1 - preset.min_amplitude) + (0.5 - horn_rot * 0.5) * preset.min_amplitude), rhorn_delay);
		right_delay.add_isample(delayed_bass /*bass_sample * ((1 - preset.min_amplitude) + (0.5 - bass_rot * 0.5) * preset.min_amplitude)*/, rbass_delay);

		//Play delay
		double l = left_delay.process();
		double r = right_delay.process();
		double ls = l + r * preset.stereo_mix;
		double rs = r + l * preset.stereo_mix;
		ls *= 2.0/(1 + preset.stereo_mix);
		rs *= 2.0/(1 + preset.stereo_mix);

		//Apply reverb
		SchroederReverbData reverb_data;
		reverb_data.delay = 0.07 + 0.13 * preset.room_size;
		reverb_data.feedback = 0.3 + 0.5 * preset.room_size;
		mix(ls, lreverb.apply(ls, reverb_data, info), preset.room_amount);
		mix(rs, rreverb.apply(rs, reverb_data, info), preset.room_amount);

		//Mix
		mix(outputs[0], ls, preset.mix);
		mix(outputs[1], rs, preset.mix);
	}

	//Rotate speakers
	horn_rotation += horn_speed.process(info.time_step) * info.time_step;
	bass_rotation -= bass_speed.process(info.time_step) * info.time_step;

	//Switch speaker speed
	RotaryState state = preset.state();
	if (curr_rotary_state != state) {
		curr_rotary_state = state;
		double horn_attack = (preset.horn_fast_frequency - preset.horn_slow_frequency)/preset.horn_slow_ramp;
		double horn_release = (preset.horn_fast_frequency - preset.horn_slow_frequency)/preset.horn_fast_ramp;
		double bass_attack = (preset.bass_fast_frequency - preset.bass_slow_frequency)/preset.bass_slow_ramp;
		double bass_release = (preset.bass_fast_frequency - preset.bass_slow_frequency)/preset.bass_fast_ramp;
		double hspeed = 0;
		double bspeed = 0;
		switch (state) {
		case RotaryState::ROTARY_SLOW:
			hspeed = preset.horn_slow_frequency;
			bspeed = preset.bass_slow_frequency;
			break;
		case RotaryState::ROTARY_STOP:
			break;
		case RotaryState::ROTARY_FAST:
			hspeed = preset.horn_fast_frequency;
			bspeed = preset.bass_fast_frequency;
			break;
		}
		horn_speed.set(hspeed, horn_attack, horn_release);
		bass_speed.set(bspeed, bass_attack, bass_release);
	}
}

Menu* RotarySpeakerEffect::create_menu() {
	return new FunctionMenu([this](){ return new EffectView(this); }, nullptr);
}

RotarySpeakerEffect::~RotarySpeakerEffect() {

}

void RotarySpeakerProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.stop.load(tree, "stop", false);
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

	preset.room_amount = tree.get<double>("room_amount", 0.3);
	preset.room_size = tree.get<double>("room_size", 0.3);
}

boost::property_tree::ptree RotarySpeakerProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("stop", preset.stop.save());
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

	tree.put("room_size", preset.room_size);
	tree.put("room_amount", preset.room_amount);

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
