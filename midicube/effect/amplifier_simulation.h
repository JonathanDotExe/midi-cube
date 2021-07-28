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
	DIGITAL_DISTORTION, POLYNOMAL_DISTORTION, ARCTAN_DISTORTION, CUBIC_DISTORTION, FUZZ_DISTORTION
};


struct AmplifierSimulationPreset {
	BindableBooleanValue on{true};
	BindableTemplateValue<double> post_gain{0, 0, 1};
	BindableTemplateValue<double> drive{0, 0, 1};
	BindableTemplateValue<double> tone{0.6, 0, 1};
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

	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	AmplifierSimulationEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);

	~AmplifierSimulationEffect();
};

#endif /* MIDICUBE_EFFECT_AMPLIFIER_SIMULATION_H_ */
