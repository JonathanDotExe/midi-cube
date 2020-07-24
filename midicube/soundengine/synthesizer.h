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

#define MAX_OSCILATORS 20


enum class FilterType {
	LOW_PASS, HIGH_PASS
};

struct FilterData {
	FilterType type;
	double cutoff;
};

struct OscilatorEnvelope {
	OscilatorSlot* osc;
	ADSREnvelope env;
	std::vector<FilterData> filters;
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

struct FilterInstance {
	double last_filtered;
	double last;
	bool started;
};

struct SynthesizerPreset {
	std::vector<OscilatorEnvelope> oscilators;
	std::vector<ADSREnvelope> envelopes;
	std::vector<EnvelopeBinding> envelope_bindings;
	std::vector<ControlBinding> control_bindings;

	~SynthesizerPreset() {
		for (size_t i = 0; i < oscilators.size() ; ++i) {
			delete oscilators[i].osc;
		}
	}
};

class Synthesizer: public SoundEngine {
private:
	SynthesizerPreset* preset = nullptr;
	std::array<std::array<FilterInstance, SOUND_ENGINE_POLYPHONY>, MAX_OSCILATORS> filters = {};
	double release_time;
public:
	Synthesizer();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, size_t note_index);

	void note_not_pressed(SampleInfo& info, TriggeredNote& note, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	void control_change(unsigned int control, unsigned int value);

	bool note_finished(SampleInfo& info, TriggeredNote& note);

	std::string get_name();

	virtual ~Synthesizer();
};

#endif /* MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_ */
