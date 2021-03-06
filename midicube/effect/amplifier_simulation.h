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

#define AMP_OVERSAMPLING 2

enum DistortionType {
	DIGITAL_DISTORTION, POLYNOMAL_DISTORTION, ARCTAN_DISTORTION
};

struct AmplifierSimulationPreset {
	bool on = false;
	double boost = 0;
	double drive = 0;
	double tone = 0.6;
	DistortionType type = ARCTAN_DISTORTION;
};

class AmplifierSimulationEffect {
private:
	Filter lfilter;
	Filter rfilter;
public:
	AmplifierSimulationEffect();
	void apply(double& lsample, double& rsample, AmplifierSimulationPreset& preset, SampleInfo& info);
	virtual ~AmplifierSimulationEffect();
};


#endif /* MIDICUBE_EFFECT_AMPLIFIER_SIMULATION_H_ */
