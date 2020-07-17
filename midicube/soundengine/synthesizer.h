/*
 * synthesizer.h
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_
#define MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_

#include "soundengine.h"
#include "../oscilator.h"
#include "../envelope.h"
#include "../synthesis.h"

struct FilterOscilator {
	Filter* filter;
	size_t osc;
};

struct EnvelopeBinding {
	std::string property;
	size_t oscilator;
	size_t envelope;
	double from;
	double to;
};

struct ControlBinding {
	std::string property;
	size_t oscilator;
	unsigned int control;
	double from;
	double to;
};

struct SynthesizerPreset {
	std::vector<OscilatorSlot*> oscilators;
	std::vector<FilterOscilator> filters;
	std::vector<ADSREnvelope> envelopes;
	std::vector<EnvelopeBinding> envelope_bindings;
	std::vector<ControlBinding> control_bindings;
};

class Synthesizer: public SoundEngine {
private:
	SynthesizerPreset* preset = nullptr;
public:
	Synthesizer();
	virtual ~Synthesizer();
};

#endif /* MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_ */
