/*
 * effect.cpp
 *
 *  Created on: Mar 30, 2021
 *      Author: jojo
 */

#include "effect.h"

const Effect* InsertEffect::get_effect() const {
	return effect;
}

void InsertEffect::set_effect(Effect *effect) {
	delete this->effect;
	this->effect = effect;
}


const Effect* MasterEffect::get_effect() const {
	return effect;
}

void MasterEffect::set_effect(Effect *effect) {
	delete this->effect;
	this->effect = effect;
}
