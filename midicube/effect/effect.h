/*
 * effect.h
 *
 *  Created on: Mar 30, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_EFFECT_H_
#define MIDICUBE_EFFECT_EFFECT_H_

#include "../audio.h"
#include "../oscilator.h"
#include "../midi.h"

class Effect {
	void apply(double& lsample, double& rsample, SampleInfo& info) = 0;
	void midi_message(MidiMessage& msg, SampleInfo& info) = 0;
	virtual ~Effect();
};


#endif /* MIDICUBE_EFFECT_EFFECT_H_ */
