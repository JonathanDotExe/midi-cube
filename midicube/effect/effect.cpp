/*
 * effect.cpp
 *
 *  Created on: Mar 30, 2021
 *      Author: jojo
 */

#include "effect.h"

void InsertEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	if (effect) {
		effect->apply(lsample, rsample, info);
	}
}

const Effect* InsertEffect::get_effect() const {
	return effect;
}

void InsertEffect::set_effect(Effect *effect) {
	delete this->effect;
	this->effect = effect;
}


void MasterEffect::apply(double& lsample, double& rsample, SampleInfo& info) {
	lsample = this->lsample;
	rsample = this->rsample;

	if (effect) {
		effect->apply(lsample, rsample, info);
	}

	this->lsample = 0;
	this->rsample = 0;
}

const Effect* MasterEffect::get_effect() const {
	return effect;
}

void MasterEffect::set_effect(Effect *effect) {
	delete this->effect;
	this->effect = effect;
}
