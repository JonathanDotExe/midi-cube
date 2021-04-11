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
