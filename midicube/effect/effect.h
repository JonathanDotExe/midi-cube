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
#include "../binding.h"

#include <unordered_map>

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

class SoundEngineDevice;

class EffectProgram {
public:
	std::unordered_map<std::string, unsigned int> ccs;

	virtual void load(pt::ptree tree) {
		auto ccs = tree.get_child_optional("ccs");
		if (ccs) {
			for (auto c : ccs.get()) {
				std::string name = c.first;
				unsigned int cc = c.second.get_value<unsigned int>(128);
				this->ccs.insert(std::pair<std::string, unsigned int>(name, cc));
			}
		}
	}

	virtual pt::ptree save() {
		pt::ptree tree;
		for (auto cc : ccs) {
			tree.add("ccs." + cc.first, cc.second);
		}
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

class EffectBuilder {
public:

	virtual Effect* build() = 0;

	virtual std::string get_name() = 0;

	virtual bool matches(Effect* effect) = 0;

	virtual EffectProgram* create_program() = 0;

	virtual ~EffectBuilder() {

	};
};

template <typename T>
std::string get_effect_name();

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
