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

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

class EffectProgram {
public:
	virtual void load(pt::ptree tree) = 0;

	virtual pt::ptree save() = 0;

	virtual ~EffectProgram() {

	}
};

class Effect {
public:
	virtual void apply(double& lsample, double& rsample, SampleInfo& info) = 0;
	virtual void midi_message(MidiMessage& msg, SampleInfo& info) {

	}
	virtual void apply_program(EffectProgram* prog) {

	}

	virtual void save_program(EffectProgram** prog) {

	}
	virtual EffectProgram* create_program() {
		return nullptr;
	}
	virtual ~Effect() {

	}
};

class EffectBuilder {
public:

	virtual Effect* build() = 0;

	virtual std::string get_name() = 0;

	virtual bool matches(Effect* effect) = 0;

	virtual ~EffectBuilder() {

	};
};

template <typename T>
std::string get_effect_name();

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
};


#endif /* MIDICUBE_EFFECT_EFFECT_H_ */
