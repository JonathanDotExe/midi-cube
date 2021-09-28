/*
 * effect.h
 *
 *  Created on: Mar 30, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_EFFECT_H_
#define MIDICUBE_EFFECT_EFFECT_H_

#include "../framework/core/audio.h"
#include "../framework/dsp/oscilator.h"
#include "../framework/core/midi.h"
#include "../framework/data/binding.h"

#include <unordered_map>

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

class SoundEngineDevice;

class EffectProgram {
public:
	virtual void load(pt::ptree tree) {

	}

	virtual pt::ptree save() {
		pt::ptree tree;
		return tree;
	}

	virtual ~EffectProgram() {

	}
};

class Effect {
protected:
	LocalMidiBindingHandler cc;

public:

	virtual void init(SoundEngineDevice& engine);

	virtual void apply(double& lsample, double& rsample, SampleInfo& info) = 0;
	virtual void apply_program(EffectProgram* prog) {

	}

	virtual void save_program(EffectProgram** prog) {

	}
	virtual ~Effect() {

	}
};

template <typename T>
EffectProgram* create_effect_program();

template <typename T>
class TemplateEffectBuilder : public EffectBuilder {
public:
	inline Effect* build() {
		return new T();
	}
	std::string get_name() {
		return get_effect_name<T>();
	}

	bool matches(Effect* effect) {
		return dynamic_cast<T*>(effect) != nullptr;
	}

	EffectProgram* create_program() {
		return create_effect_program<T>();
	}
};


#endif /* MIDICUBE_EFFECT_EFFECT_H_ */
