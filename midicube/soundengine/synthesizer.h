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

struct OscilatorEnvelope {
	OscilatorSlot* osc;
	ADSREnvelope env;
};

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
	std::vector<OscilatorEnvelope> oscilators;
	std::vector<FilterOscilator> filters;
	std::vector<ADSREnvelope> envelopes;
	std::vector<EnvelopeBinding> envelope_bindings;
	std::vector<ControlBinding> control_bindings;

	~SynthesizerPreset() {
		for (size_t i = 0; i < oscilators.size() ; ++i) {
			delete oscilators[i].osc;
		}
		for (size_t i = 0; i < filters.size() ; ++i) {
			delete filters[i].filter;
		}
	}
};

class Synthesizer: public SoundEngine {
private:
	SynthesizerPreset* preset = nullptr;
	std::vector<double> samples;
	double release_time;
public:
	Synthesizer();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, TriggeredNote& note);

	std::string get_name() = 0;

	virtual ~Synthesizer();
};

#endif /* MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_ */
