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
#define MAX_COMPONENTS 100

enum class BindingType {
	SET, ADD, MUL
};

struct ComponentPropertyBinding {
	BindingType type;
	size_t property;
	size_t component;
};

class SynthComponent {
public:
	virtual double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) = 0;
	virtual void set_property(size_t index, double value, BindingType type) = 0;
	virtual std::vector<std::string> property_names() = 0;
	virtual size_t property_count() = 0;
	virtual ~SynthComponent() {

	}
};

class OscilatorComponent {
private:
	double phase_shift = 0;
public:
	AnalogOscilator osc;
	unsigned int unison_amount = 0;
	double semi = 0;				//Static pitch offset in semitones
	double transpose = 1;		//Static pitch offset as frequency multiplier
	double volume = 1;
	double sync = 1;			//Sync frequency factor
	double fm = 0;				//Dynamic pitch offset in time shift
	double pitch = 0;			//Dynamic pitch offset in semitones
	double unison_detune = 0.1;

	double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env);
	void set_property(size_t index, double value, BindingType type);
	std::vector<std::string> property_names();
	size_t property_count();
};

class ComponentSlot {
private:
	SynthComponent* comp;
	std::vector<ComponentPropertyBinding> bindings;
public:
	double process(std::array<double, MAX_COMPONENTS>& values, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env);
	void set_component(SynthComponent* comp);
	SynthComponent* get_component();
};

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
	std::array<std::array<std::array<FilterInstance, 2>, SOUND_ENGINE_POLYPHONY>, MAX_OSCILATORS> filters = {};
	double release_time;
public:
	Synthesizer();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data, size_t note_index);

	void note_not_pressed(SampleInfo& info, TriggeredNote& note, SoundEngineData& data, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status, SoundEngineData& data);

	void control_change(unsigned int control, unsigned int value, SoundEngineData& data);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data);

	std::string get_name();

	virtual ~Synthesizer();
};

#endif /* MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_ */
