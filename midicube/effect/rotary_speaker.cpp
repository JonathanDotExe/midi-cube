/*
 * RotarySpeaker.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: jojo
 */

#include "rotary_speaker.h"

RotarySpeakerEffect::RotarySpeakerEffect() {
	// TODO Auto-generated constructor stub

}

void RotarySpeakerEffect::apply(double& lsample, double& rsample, RotarySpeakerPreset& preset, SampleInfo& info) {
	if (preset.on) {
		//Sum samples up
		double sample = lsample + rsample;
		//

	}
}

RotarySpeakerEffect::~RotarySpeakerEffect() {
	// TODO Auto-generated destructor stub
}

