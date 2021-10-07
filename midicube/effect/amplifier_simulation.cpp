/*
 * amplifier_simulation.cpp
 *
 *  Created on: Jan 7, 2021
 *      Author: jojo
 */

#include "amplifier_simulation.h"

AmplifierSimulationEffect::AmplifierSimulationEffect() {
	cc.add_binding(&preset.on);
	cc.add_binding(&preset.post_gain);
	cc.add_binding(&preset.drive);
	cc.add_binding(&preset.tone);
}

static inline double cubic_distortion(double sample) {
	if (sample < 0) {
		double t = sample + 1;
		sample = t * t * t - 1;
	}
	else {
		double t = sample - 1;
		sample = t * t * t + 1;
	}
	return sample;
}

static double apply_distortion(double sample, double drive, DistortionType type) {
	switch (type) {
	/*case DistortionType::TUBE_AMP_DISTORTION:
	{
		double a = sin((drive * 100.0 + 1)/102 * (M_PI/2.0));
		double k = 2 * a/(1 - a);
		sample = (1 + k) * sample / (1 + k * abs(sample));
	}
		break;*/
	case DistortionType::DIGITAL_DISTORTION:
	{
		double clip = (1 - drive);
		sample = fmax(fmin(sample, clip), -clip);
		sample *= clip ? 1/clip : 0;
	}
		break;
	case DistortionType::POLYNOMAL_DISTORTION:
	{
		sample -= (sample * sample * sample) * drive;
	}
		break;
	case DistortionType::ARCTAN_DISTORTION:
	{
		sample = atan(sample * (1 + drive * 4));
	}
		break;
	case DistortionType::CUBIC_DISTORTION:
	{
		sample = cubic_distortion(sample * (0.3 + drive * 4.5));
	}
	break;
	case DistortionType::FUZZ_DISTORTION:
	{
		sample = cubic_distortion(cubic_distortion(cubic_distortion(sample * (0.3 + drive * 2)))) / 3.0;
	}
	break;
	}
	return sample;
}

void AmplifierSimulationEffect::process(const SampleInfo &info) {
	double lsample = inputs[0];
	double rsample = inputs[1];
	if (preset.on) {
		//Distortion
		lsample = apply_distortion(lsample, preset.drive, preset.type);
		rsample = apply_distortion(rsample, preset.drive, preset.type);

		//Low-pass
		FilterData data;
		data.type = FilterType::LP_24;
		data.cutoff = 200 + preset.tone * 20000;
		lsample = lfilter.apply(data, lsample, info.time_step);
		rsample = rfilter.apply(data, rsample, info.time_step);

		//Gain
		double gain = preset.post_gain + 1;
		lsample *= gain;
		rsample *= gain;
	}
	outputs[0] = lsample;
	outputs[1] = rsample;
}

AmplifierSimulationEffect::~AmplifierSimulationEffect() {

}

void AmplifierSimulationProgram::load(boost::property_tree::ptree tree) {
	preset.on.load(tree, "on", true);
	preset.post_gain.load(tree, "post_gain", 0);
	preset.drive.load(tree, "drive", 0);
	preset.tone.load(tree, "tone", 0.6);
}

boost::property_tree::ptree AmplifierSimulationProgram::save() {
	boost::property_tree::ptree tree;
	tree.add_child("on", preset.on.save());
	tree.add_child("post_gain", preset.post_gain.save());
	tree.add_child("drive", preset.drive.save());
	tree.add_child("tone", preset.tone.save());
	return tree;
}


void AmplifierSimulationEffect::save_program(PluginProgram **prog) {
	AmplifierSimulationProgram* p = dynamic_cast<AmplifierSimulationProgram*>(*prog);
	//Create new
	if (!p) {
		delete *prog;
		p = new AmplifierSimulationProgram();
	}
	p->preset = preset;
	*prog = p;
}

void AmplifierSimulationEffect::apply_program(PluginProgram *prog) {
	AmplifierSimulationProgram* p = dynamic_cast<AmplifierSimulationProgram*>(prog);
	//Create new
	if (p) {
		preset = p->preset;
	}
	else {
		preset = {};
	}
}

std::string AmplifierSimulationProgram::get_plugin_name() {
	return AMPLIFIER_SIMULATION_IDENTIFIER;
}
