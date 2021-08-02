/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "delay.h"
#include <cmath>

DelayEffect::DelayEffect() {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.left_init_delay_offset);
	cc.add_binding(&preset.left_delay);
	cc.add_binding(&preset.left_feedback);
	cc.add_binding(&preset.right_init_delay_offset);
	cc.add_binding(&preset.right_delay);
	cc.add_binding(&preset.right_feedback);
	cc.add_binding(&preset.stereo);
	cc.add_binding(&preset.mix);
}

void DelayEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		double l = ldelay.process();
		double r = rdelay.process();

		unsigned int left_delay = preset.left_delay * info.sample_rate;
		unsigned int left_init_delay = fmax(0, preset.left_init_delay_offset + preset.left_delay) * info.sample_rate;
		double left_feedback = preset.left_feedback;
		unsigned int right_delay = left_delay;
		unsigned int right_init_delay = left_init_delay;
		double right_feedback = left_feedback;
		if (preset.stereo) {
			right_delay = preset.right_delay * info.sample_rate;
			right_init_delay = fmax(0, preset.right_init_delay_offset + preset.right_delay) * info.sample_rate;
			right_feedback = preset.right_feedback;
		}

		//Delay
		ldelay.add_sample(lsample, left_init_delay);
		rdelay.add_sample(rsample, right_init_delay);
		//Feedback
		ldelay.add_sample(l * left_feedback, left_delay);
		rdelay.add_sample(r * right_feedback, right_delay);

		//Mix
		lsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);
		rsample *= 1 - (fmax(0, preset.mix - 0.5) * 2);

		lsample += l * fmin(0.5, preset.mix) * 2;
		rsample += r * fmin(0.5, preset.mix) * 2;
	}
}

DelayEffect::~DelayEffect() {

}

template<>
std::string get_effect_name<DelayEffect>() {
	return "Delay";
}

template <>
EffectProgram* create_effect_program<DelayEffect>() {
	return new DelayProgram();
}


void DelayProgram::load(boost::property_tree::ptree tree) {
	EffectProgram::load(tree);
	preset.on.load(tree, "on", true);
	preset.mix.load(tree, "mix", 0.5);
	preset.left_delay.load(tree, "left_delay", 0.1);
	preset.left_init_delay_offset.load(tree, "left_init_delay_offset", 0.0);
	preset.left_feedback.load(tree, "left_feedback", 0.2);
	preset.right_delay.load(tree, "right_delay", 0.1);
	preset.right_init_delay_offset.load(tree, "right_init_delay_offset", 0.0);
	preset.right_feedback.load(tree, "right_feedback", 0.2);
	preset.on.load(tree, "stereo", false);
}

boost::property_tree::ptree DelayProgram::save() {
	boost::property_tree::ptree tree = EffectProgram::save();
	tree.put("on", preset.on);
	tree.put("mix", preset.mix);
	tree.put("left_delay", preset.left_delay);
	tree.put("left_init_delay_offset", preset.left_init_delay_offset);
	tree.put("left_feedback", preset.left_feedback);
	tree.put("right_delay", preset.right_delay);
	tree.put("right_init_delay_offset", preset.right_init_delay_offset);
	tree.put("right_feedback", preset.right_feedback);
	tree.put("stereo", preset.stereo);
	return tree;
}

void DelayEffect::save_program(EffectProgram **prog) {
	DelayProgram* p = dynamic_cast<DelayProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new DelayProgram();
	}
	
	p->preset = preset;
	*prog = p;
}

void DelayEffect::apply_program(EffectProgram *prog) {
	DelayProgram* p = dynamic_cast<DelayProgram*>(prog);
	//Create new
	if (p) {
		
		preset = p->preset;
	}
	else {
		
		preset = {};
	}
}
