/*
 * effect.cpp
 *
 *  Created on: Oct 6, 2021
 *      Author: jojo
 */

#include "effect.h"

void Effect::apply_program(PluginProgram *prog) {
}

void Effect::save_program(PluginProgram **prog) {
}

void Effect::recieve_midi(const MidiMessage &message, const SampleInfo &info) {
}

void Effect::cleanup() {
	cc.unbind();
}
