/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "bitcrusher.h"
#include <cmath>

BitCrusherEffect::BitCrusherEffect() {

}

void BitCrusherEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (preset.on) {
		double accuracy = pow(2, preset.bits - 1);
		lsample = (int) (lsample * accuracy)/(accuracy);
		rsample = (int) (rsample * accuracy)/(accuracy);
	}
}

BitCrusherEffect::~BitCrusherEffect() {

}

template<>
std::string get_effect_name<BitCrusherEffect>() {
	return "Bit Crusher";
}

void BitCrusherProgram::load(boost::property_tree::ptree tree) {
	preset.on = tree.get<bool>("on", true);
	preset.bits = tree.get<unsigned int>("bits", 16);
}

boost::property_tree::ptree BitCrusherProgram::save() {
	boost::property_tree::ptree tree;
	tree.put("on", preset.on);
	tree.put("bits", preset.bits);
	return tree;
}
