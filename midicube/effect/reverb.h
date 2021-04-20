/*
 * reverb.h
 *
 *  Created on: Apr 19, 2021
 *      Author: jojo
 */


#include "effect.h"

#include "../synthesis.h"

#define REVERB_COMB_FILTERS 4
#define REVERB_ALLPASS_FILTERS 2

struct ReverbPreset {
	bool on = true;
	double delay = 0.1;
	double decay = 0.5;
	double mix = 0.5;

	std::array<double, REVERB_COMB_FILTERS> comb_delay_diff = {0, -0.01173, 0.01931, -0.00797};
	std::array<double, REVERB_COMB_FILTERS> comb_decay_diff = {0, -0.1313, 0.02743, -0.31};

	double allpass_delay = 0.09;
	double allpass_decay = 0.015;
};

class CombFilter {
private:
	DelayBuffer delay;

public:

	double process(double in, double gain, unsigned int delay);
};

class AllPassFilter {
private:
	DelayBuffer indelay;
	DelayBuffer delay;

public:

	double process(double in, double gain, unsigned int delay);
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
	std::array<CombFilter, 4> lcomb_filters;
	std::array<CombFilter, 4> rcomb_filters;
	std::array<AllPassFilter, 2> lallpass_filters;
	std::array<AllPassFilter, 2> rallpass_filters;

public:
	ReverbPreset preset;

	ReverbEffect();
	void apply(double& lsample, double& rsample, SampleInfo& info);
	void save_program(EffectProgram **prog);
	void apply_program(EffectProgram *prog);
	~ReverbEffect();
};
