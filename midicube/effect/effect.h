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
public:
	virtual void apply(double& lsample, double& rsample, SampleInfo& info) = 0;
	virtual void midi_message(MidiMessage& msg, SampleInfo& info) = 0;
	virtual ~Effect() {

	}
};

class InsertEffect {
private:
	Effect* effect = nullptr;

public:
	InsertEffect() {

	}
	
	void apply(double& lsample, double& rsample, SampleInfo& info);

	void set_effect(Effect *effect = nullptr);

	const Effect* get_effect() const;
	
	~InsertEffect();
};

class MasterEffect {
private:
	Effect* effect = nullptr;

public:
	double lsample = 0;
	double rsample = 0;
	size_t next_effect = 0;

	MasterEffect() {

	}

	void apply(double& lsample, double& rsample, SampleInfo& info);

	void set_effect(Effect *effect = nullptr);

	const Effect* get_effect() const;

	~MasterEffect();
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
