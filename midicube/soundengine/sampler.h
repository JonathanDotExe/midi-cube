/*
 * sampler.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SAMPLER_H_
#define MIDICUBE_SOUNDENGINE_SAMPLER_H_

#include "soundengine.h"
#include "../audiofile.h"
#include "../envelope.h"
#include "../util.h"
#include "../audioloader.h"
#include <array>

namespace pt = boost::property_tree;

#define MIDI_NOTES 128

struct ModulateableProperty {
	double value = 0;
	unsigned int cc = 0;
	double cc_amount = 0;
	double cc_multiplier = 1;
	double velocity_amount = 0;

	inline double apply_modulation(double velocity, std::array<double, MIDI_CONTROL_COUNT>& cc_val) {
		double val = value;
		val += velocity * velocity_amount;
		val += cc_val[cc] * cc_amount;
		if (cc_multiplier != 1) {
			val *= (1 - cc_val[cc]) + cc_val[cc] * cc_multiplier;
		}
		return val;
	}

	void load(boost::property_tree::ptree& parent, std::string path, double def) {
		auto tree = parent.get_child_optional(path);
		if (tree && tree.get().get_child_optional("value")) {
			load(tree.get(), def);
		}
		else {
			value = parent.get<double>(path, def);
			cc = 0;
			cc_amount = 0;
			cc_multiplier = 1;
			velocity_amount = 0;
		}
	}

	void load(boost::property_tree::ptree& tree, double def) {
		value = tree.get("value", def);
		cc = tree.get("cc", cc);
		cc_amount = tree.get("cc_amount", cc_amount);
		cc_multiplier = tree.get("cc_multiplier", cc_multiplier);
		velocity_amount = tree.get("velocity_amount", velocity_amount);
	}

};

struct ModulatableADSREnvelopeData {
	ModulateableProperty attack{0.0005};
	ModulateableProperty decay{0};
	ModulateableProperty sustain{1};
	ModulateableProperty release{0.0005};

	ADSREnvelopeShape attack_shape = ADSREnvelopeShape::LINEAR_ADSR;
	ADSREnvelopeShape pre_decay_shape = ADSREnvelopeShape::ANALOG_ADSR;
	ADSREnvelopeShape decay_shape = ADSREnvelopeShape::ANALOG_ADSR;
	ADSREnvelopeShape release_shape = ADSREnvelopeShape::ANALOG_ADSR;

	ModulateableProperty hold{0};
	ModulateableProperty pre_decay{0};

	ModulateableProperty attack_hold{0};
	ModulateableProperty peak_volume{1};
	ModulateableProperty decay_volume{1};
	ModulateableProperty sustain_time{0};
	ModulateableProperty release_volume{0};

	bool pedal_catch = false;

	inline ADSREnvelopeData apply(double velocity, std::array<double, MIDI_CONTROL_COUNT>& cc_val) {
		ADSREnvelopeData env;
		env.attack = attack.apply_modulation(velocity, cc_val);
		env.decay = decay.apply_modulation(velocity, cc_val);
		env.sustain = sustain.apply_modulation(velocity, cc_val);
		env.release = release.apply_modulation(velocity, cc_val);

		env.attack_shape = attack_shape;
		env.pre_decay_shape = pre_decay_shape;
		env.decay_shape = decay_shape;
		env.release_shape = release_shape;

		env.hold = hold.apply_modulation(velocity, cc_val);
		env.pre_decay =  pre_decay.apply_modulation(velocity, cc_val);

		env.attack_hold = attack_hold.apply_modulation(velocity, cc_val);
		env.peak_volume = peak_volume.apply_modulation(velocity, cc_val);
		env.decay_volume = decay_volume.apply_modulation(velocity, cc_val);
		env.sustain_time = sustain_time.apply_modulation(velocity, cc_val);
		env.release_volume = release_volume.apply_modulation(velocity, cc_val);
		return env;
	}
};


enum LoopType {
	NO_LOOP, ATTACK_LOOP, ALWAYS_LOOP
};

enum TriggerType {
	ATTACK_TRIGGER, RELEASE_TRIGGER
};

struct SampleFilter {
	bool on = false;
	FilterType filter_type = FilterType::LP_12;
	ModulateableProperty filter_cutoff{1};
	ModulateableProperty filter_resonance{0};
	double filter_kb_track = 0;
	unsigned int filter_kb_track_note = 36;
};

struct SampleEnvelope {
	ModulatableADSREnvelopeData env = {0, 0, 1, 0.001, LINEAR_ADSR, EXPONENTIAL_ADSR, EXPONENTIAL_ADSR, EXPONENTIAL_ADSR};
	ModulateableProperty velocity_amount{0.0};
	bool sustain_entire_sample = false;
};

struct LoopedAudioSample {
	StreamedAudioSample* sample = nullptr;
	unsigned int start = 0;
	unsigned int loop_start = 0;
	unsigned int loop_end = 0;
	unsigned int loop_crossfade = 0;
};

struct SampleRegion {
	unsigned int min_velocity = 0;
	unsigned int max_velocity = 127;
	unsigned int min_note = 0;
	unsigned int max_note = 127;
	double note = 60;
	ModulateableProperty volume{1};
	double pitch_keytrack = 1;
	double release_decay = 1;

	LoopedAudioSample sample;
	LoopType loop = NO_LOOP;

	SampleFilter filter;
	SampleEnvelope env;

	TriggerType trigger = TriggerType::ATTACK_TRIGGER;

	SampleRegion() {

	};
};

struct SamplerVoice : public TriggeredNote {
	double time = 0;
	bool hit_loop = false;
	double layer_amp = 1;
	SampleRegion* region = nullptr;
	LoopedAudioSample* sample = nullptr;
	WaveTableADSREnvelope env;
	Filter lfilter;
	Filter rfilter;
	size_t current_buffer = 0;
	size_t floor_block = 0;
	size_t ceil_block = 0;
};

struct SampleRegionIndex {
	std::array<std::array<std::vector<SampleRegion*>, MIDI_NOTES>, MIDI_NOTES> velocities;
};

class SampleSound {
public:
	std::string default_path = "./";
	std::string name = "Sample";
	double volume = 1;
	std::vector<SampleRegion*> samples = {};

	SampleSound();

	~SampleSound();

};

class SamplerProgram : public EngineProgram {
public:
	std::string sound_name;

	virtual void load(boost::property_tree::ptree tree);
	virtual boost::property_tree::ptree save();

	virtual ~SamplerProgram() {

	}
};

class SampleSoundStore {
private:
	std::vector<SampleSound*> samples;
public:
	StreamedAudioPool pool;

	SampleSound* get_sound(size_t index);

	SampleSound* get_sound(std::string name);

	void load_sounds(std::string folder);

	std::vector<SampleSound*> get_sounds();

	~SampleSoundStore();
};

extern SampleSoundStore global_sample_store;

#define SAMPLER_POLYPHONY 64

class Sampler : public BaseSoundEngine<SamplerVoice, SAMPLER_POLYPHONY> {

private:
	SampleSound* sample;
	SampleRegionIndex index;
	std::array<double, MIDI_CONTROL_COUNT> cc;

	void set_sample (SampleSound* sample);

public:

	Sampler();

	void process_note_sample(double& lsample, double& rsample, SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index);

	bool note_finished(SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index);

	void press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit);

	void release_note(SampleInfo& info, unsigned int note);

	std::string get_name();

	ssize_t get_sound_index();

	void set_sound_index(ssize_t index);

	void save_program(EngineProgram **prog);

	void apply_program(EngineProgram *prog);

	~Sampler();

};

extern SampleSound* load_sound(std::string file, std::string folder, StreamedAudioPool& pool);

extern void save_sound(std::string file);


#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
