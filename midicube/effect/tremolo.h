/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_TREMOLO_H_
#define MIDICUBE_EFFECT_TREMOLO_H_

#include "../audio.h"
#include "../oscilator.h"
#include "effect.h"

struct TremoloPreset {
	bool on = true;
	double rate = 2;
	double depth = 0.5;
	AnalogWaveForm waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class TremoloProgram : public EffectProgram {
public:
	TremoloPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~TremoloProgram() {

	}
};

class TremoloEffect : public Effect {
private:
	AnalogOscilator osc;

public:
	TremoloPreset preset;

	TremoloEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~TremoloEffect();
};

#endif /* MIDICUBE_EFFECT_TREMOLO_H_ */
