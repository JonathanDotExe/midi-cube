/*
 * synthesizer.h
 *
 *  Created on: 15 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_
#define MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_

#include <type_traits>
#include <atomic>
#include <algorithm>

#include "soundengine.h"
#include "../oscilator.h"
#include "../envelope.h"
#include "../synthesis.h"
#include "../filter.h"


#define MAX_COMPONENTS 25

enum class BindingType {
	SET, ADD, MUL
};

struct ComponentPropertyBinding {
	BindingType type;
	size_t property;
	size_t component;
	double from;
	double to;
};

class SynthComponent {
public:
	virtual double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) = 0;
	virtual void set_property(size_t index, double value) = 0;
	virtual void add_property(size_t index, double value) = 0;
	virtual void mul_property(size_t index, double value) = 0;
	virtual void reset_properties() = 0;
	virtual std::vector<std::string> property_names() = 0;
	virtual size_t property_count() = 0;
	virtual double from() = 0;
	virtual double to() = 0;
	virtual bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
		return !note.pressed;
	}
	virtual void control_change(unsigned int control, unsigned int value) {

	}
	virtual ~SynthComponent() {

	}
};

class OscilatorComponent : public SynthComponent {
public:
	AnalogOscilatorBank<SOUND_ENGINE_POLYPHONY> osc;
	double semi = 0;			//Static pitch offset in semitones
	double transpose = 1;		//Static pitch offset as frequency multiplier
	double volume = 1;
	double amplitude = 1;
	double unison_detune = 0.1;	//Set the unison amount here instead of using data.unison detune which is used internally
	double sync = 1;			//Sync frequency factor
	double sync_mod = 1;		//Sync frequency factor (modulated)
	double fm = 1;				//Dynamic pitch offset in frequency multiplier
	double pitch = 0;			//Dynamic pitch offset in semitones
	double unison_detune_mod = 0.1;
	double pulse_width = 0.5;
	double pulse_width_mod = 0.5;

	OscilatorComponent();
	double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);
	void set_property(size_t index, double value);
	void add_property(size_t index, double value);
	void mul_property(size_t index, double value);
	double from();
	double to();
	void reset_properties();
	std::vector<std::string> property_names();
	size_t property_count();
};

class AmpEnvelopeComponent : public SynthComponent {
private:
	double input = 0;
	double amplitude_mod = 0;
public:
	double amplitude = 1;
	ADSREnvelope envelope; //TODO Modulation for envelope parameters

	double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);
	void set_property(size_t index, double value);
	void add_property(size_t index, double value);
	void mul_property(size_t index, double value);
	double from();
	double to();
	void reset_properties();
	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env);
	std::vector<std::string> property_names();
	size_t property_count();
};

class ModEnvelopeComponent : public SynthComponent {
private:
	double amplitude_mod = 1;
public:
	double amplitude = 1;
	ADSREnvelope envelope; //TODO Modulation for envelope parameters

	double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);
	void set_property(size_t index, double value);
	void add_property(size_t index, double value);
	void mul_property(size_t index, double value);
	double from();
	double to();
	void reset_properties();
	std::vector<std::string> property_names();
	size_t property_count();
};

static std::vector<std::string> filter_properties = {"Input", "Cutoff"};

#define FILTER_INPUT_PROPERTY 0
#define FILTER_CUTOFF_PROPERTY 1

template<typename T>
class FilterComponent : public SynthComponent {

private:
	std::array<T, SOUND_ENGINE_POLYPHONY> filters = {};
	double cutoff_mod = 21000;
public:
	double input = 0;
	double cutoff = 21000;
	double keyboard_tracking = 0;

	double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
		T& filter = filters.at(note_index);
		if (keyboard_tracking) {
			cutoff_mod *= 1 + std::max(0.0, ((double) note.note - 36)/12.0 * keyboard_tracking);
		}
		filter.set_cutoff(cutoff_mod); //TODO do something more performant than updating every frame
		return filter.apply(input, info.time_step);
	}

	void set_property(size_t index, double value) {
		switch (index) {
		case FILTER_INPUT_PROPERTY:
			input = value;
			break;
		case FILTER_CUTOFF_PROPERTY:
			cutoff_mod = value;
			break;
		}
	}

	void add_property(size_t index, double value) {
		switch (index) {
		case FILTER_INPUT_PROPERTY:
			input += value;
			break;
		case FILTER_CUTOFF_PROPERTY:
			cutoff_mod += value;
			break;
		}
	}

	void mul_property(size_t index, double value) {
		switch (index) {
		case FILTER_INPUT_PROPERTY:
			input *= value;
			break;
		case FILTER_CUTOFF_PROPERTY:
			cutoff_mod *= value;
			break;
		}
	}

	double from() {
		return -1;
	}

	double to() {
		return 1;
	}

	void reset_properties() {
		input = 0;
		cutoff_mod = cutoff;
	}
	std::vector<std::string> property_names() {
		return filter_properties;
	}

	size_t property_count() {
		return filter_properties.size();
	}

	~FilterComponent() {

	}
};

class LowPassFilter12Component : public FilterComponent<LowPassFilter<2>> {

};

class LowPassFilter24Component : public FilterComponent<LowPassFilter<4>> {

};

class HighPassFilter12Component : public FilterComponent<HighPassFilter<2>> {

};

class HighPassFilter24Component : public FilterComponent<HighPassFilter<4>> {

};

//LFO
class LFOComponent : public SynthComponent {
private:
	double amplitude_mod = 1;
public:
	AnalogOscilatorBank<SOUND_ENGINE_POLYPHONY> osc;
	double freq = 1;	//TODO option to morph frequency
	double amplitude = 1;

	LFOComponent();
	double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);
	void set_property(size_t index, double value);
	void add_property(size_t index, double value);
	void mul_property(size_t index, double value);
	double from();
	double to();
	void reset_properties();
	std::vector<std::string> property_names();
	size_t property_count();
};

//MIDI Control
class ControlChangeComponent : public SynthComponent {
public:
	unsigned int control = 1;
	unsigned int start = 0;
	unsigned int end = 127;
	double value = 0.0;

	double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);
	void set_property(size_t index, double value);
	void add_property(size_t index, double value);
	void mul_property(size_t index, double value);
	double from();
	double to();
	void reset_properties();
	void control_change(unsigned int control, unsigned int value);
	std::vector<std::string> property_names();
	size_t property_count();
};

//Velocity
class VelocityComponent : public SynthComponent {
public:
	double start = 0;
	double end = 1;

	double process(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);
	void set_property(size_t index, double value);
	void add_property(size_t index, double value);
	void mul_property(size_t index, double value);
	double from();
	double to();
	void reset_properties();
	std::vector<std::string> property_names();
	size_t property_count();
};


class ComponentSlot {
private:
	SynthComponent* comp = nullptr;
public:
	std::vector<ComponentPropertyBinding> bindings;
	bool audible = false;

	double process(std::array<ComponentSlot, MAX_COMPONENTS>& slots, std::array<double, MAX_COMPONENTS>& values, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);
	void control_change(unsigned int control, unsigned int value);
	void set_component(SynthComponent* comp);
	SynthComponent* get_component();
	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env);
	double from();
	double to();
	~ComponentSlot();
};

struct SynthesizerPreset {
	std::array<ComponentSlot, MAX_COMPONENTS> components;
	unsigned int preset_cc = 17; //Until there is a proper way to create sounds using GUI and save presets
};

class SynthesizerData : public SoundEngineData {
public:
	SynthesizerPreset preset;
	std::atomic<int> update_preset{-1}; 	//TODO normally a mutex for the components would be better but since this feature is only temporary it is solved this way

	SynthesizerData();

	SoundEngineData* copy () {
		return new SynthesizerData(); //TODO
	}
};

class Synthesizer: public SoundEngine {

public:
	Synthesizer();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data, size_t note_index);

	void note_not_pressed(SampleInfo& info, TriggeredNote& note, SoundEngineData& data, size_t note_index);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status, SoundEngineData& data);

	void control_change(unsigned int control, unsigned int value, SoundEngineData& data);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& data);

	std::string get_name();

	SoundEngineData* create_data() {
		return new SynthesizerData();
	}

	virtual ~Synthesizer();
};

#endif /* MIDICUBE_SOUNDENGINE_SYNTHESIZER_H_ */
