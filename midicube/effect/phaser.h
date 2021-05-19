/*
 * BitCrusher.h
 *
 *  Created on: Jan 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_EFFECT_PHASER_H_
#define MIDICUBE_EFFECT_PHASER_H_

#include "../audio.h"
#include "../oscilator.h"
#include "effect.h"

struct PhaserPreset {
	bool on = true;
	double center_cutoff = 0.5;
	double lfo_depth = 0.5;
	double lfo_rate = 1;
	double mix = 0.5;
	AnalogWaveForm vibrato_waveform = AnalogWaveForm::TRIANGLE_WAVE;
};


class PhaserProgram : public EffectProgram {
public:
	PhaserPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~PhaserProgram() {

	}
};

class PhaserEffect : public Effect {
private:
	AnalogOscilator osc;
	Filter lfilter;
	Filter rfilter;

public:
	PhaserPreset preset;

	PhaserEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~PhaserEffect();
};

#endif /* MIDICUBE_EFFECT_PHASER_H_ */
