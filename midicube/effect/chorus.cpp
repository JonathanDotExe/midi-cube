/*
 * BitCrusher.cpp
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#include "chorus.h"
#include <cmath>

ChorusEffect::ChorusEffect() {

}

void ChorusEffect::apply(double& lsample, double& rsample, ChorusPreset& preset, SampleInfo& info) {
	if (preset.on) {
		double l = ldelay.process();
		double r = rdelay.process();


	}
}

ChorusEffect::~ChorusEffect() {

}

