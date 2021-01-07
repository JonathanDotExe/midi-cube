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

enum DistortionType {
	TUBE_AMP_DISTORTION, SOFT_CLIPPING_1, SOFT_CLIPPING_2, DIGITAL_DISTORTION
};

struct AmplifierSimulationPreset {
	bool on = false;
	double boost{1.0};
	double drive;
	double tone = 0.5;
	DistortionType type;
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
