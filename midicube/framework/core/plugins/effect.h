/*
 * effect.h
 *
 *  Created on: Oct 6, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_CORE_PLUGINS_EFFECT_H_
#define MIDICUBE_FRAMEWORK_CORE_PLUGINS_EFFECT_H_

#include "../plugin.h"

class Effect : public PluginInstance {
protected:
	LocalMidiBindingHandler cc;

public:

	virtual void apply_program(PluginProgram *prog);
	virtual void process(const SampleInfo &info) = 0;
	virtual void save_program(PluginProgram **prog);
	virtual void recieve_midi(const MidiMessage &message, const SampleInfo &info);

	virtual ~Effect() {

	}
};


#endif /* MIDICUBE_FRAMEWORK_CORE_PLUGINS_EFFECT_H_ */
