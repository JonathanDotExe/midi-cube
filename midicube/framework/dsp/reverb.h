/*
 * reverb.h
 *
 *  Created on: 16 Dec 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_DSP_REVERB_H_
#define MIDICUBE_FRAMEWORK_DSP_REVERB_H_

#include "../../framework/core/audio.h"
#include "../../framework/dsp/synthesis.h"
#include "../../framework/dsp/filter.h"

#define REVERB_COMB_FILTERS 4
#define REVERB_ALLPASS_FILTERS 2


class ReverbCombFilter {
private:
	DelayBuffer delay;

public:

	double process(double in, double gain, unsigned int delay);
};

class ReverbAllPassFilter {
private:
	DelayBuffer indelay;
	DelayBuffer delay;

public:

	double process(double in, double gain, unsigned int delay);
};

struct SchroederReverbData {
	double delay = 0;
	double feedback = 0;
};

class SchroederReverb {
private:
	std::array<ReverbCombFilter, REVERB_COMB_FILTERS> comb_filters;
	std::array<ReverbAllPassFilter, REVERB_ALLPASS_FILTERS> allpass_filters;

	std::array<double, REVERB_COMB_FILTERS> comb_delay_mul = {1, 1.13287, 0.6812463, 0.622141};
	std::array<double, REVERB_COMB_FILTERS> comb_decay_mul = {0.57962, 0.55271987, 0.981233, 1};

	double allpass_delay = 0.09;
	double allpass_decay = 0.015;

public:

	double apply(double sample, const SchroederReverbData& data, const SampleInfo& info);
};



#endif /* MIDICUBE_FRAMEWORK_DSP_REVERB_H_ */
