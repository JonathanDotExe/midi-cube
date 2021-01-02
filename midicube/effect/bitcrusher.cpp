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

void BitCrusherEffect::apply(double& lsample, double& rsample, BitCrusherPreset& preset, SampleInfo& info) {
	if (preset.on) {
		int accuracy = pow(2, preset.bits);
		lsample = (int) (lsample * accuracy)/((double) accuracy);
		rsample = (int) (rsample * accuracy)/((double) accuracy);
	}
}

BitCrusherEffect::~BitCrusherEffect() {

}

