/*
 * amplifier_simulation.h
 *
 *  Created on: Jan 7, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_AMPLIFIER_SIMULATION_H_
#define MIDICUBE_EFFECT_AMPLIFIER_SIMULATION_H_

#include "../audio.h"
#include "../filter.h"
#include "../synthesis.h"
#include "effect.h"

#define AMP_OVERSAMPLING 2

enum DistortionType {
	DIGITAL_DISTORTION, POLYNOMAL_DISTORTION, ARCTAN_DISTORTION
};



struct AmplifierSimulationPreset {
	bool on = true;
	double boost = 0;
	double drive = 0;
	double tone = 0.6;
	DistortionType type = ARCTAN_DISTORTION;
};

class AmplifierSimulationProgram : public EffectProgram {
public:
	AmplifierSimulationPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~AmplifierSimulationProgram() {

	}
};

class AmplifierSimulationEffect : public Effect {
private:
	Filter lfilter;
	Filter rfilter;
public:
	AmplifierSimulationPreset preset;

	EffectProgram* create_program();
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	AmplifierSimulationEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);

	~AmplifierSimulationEffect();
};


#endif /* MIDICUBE_EFFECT_AMPLIFIER_SIMULATION_H_ */
