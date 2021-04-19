/*
 * reverb.h
 *
 *  Created on: Apr 19, 2021
 *      Author: jojo
 */


#include "effect.h"

struct ReverbPreset {
	bool on = true;
	double delay = 0.5;
	double decay = 0.5;
	double mix = 0.5;
};


class ReverbProgram : public EffectProgram {
public:
	ReverbPreset preset;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~ReverbProgram() {

	}
};

class ReverbEffect : public Effect {
private:
	//TODO filters

public:
	ReverbPreset preset;

	ReverbEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	EffectProgram* create_program();
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~ReverbEffect();
};
