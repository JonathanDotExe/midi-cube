/*
 * sampler.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SAMPLER_H_
#define MIDICUBE_SOUNDENGINE_SAMPLER_H_

#include "../framework/core/plugins/soundengine.h"
#include "../framework/util/audiofile.h"
#include "../framework/dsp/envelope.h"
#include "../framework/util/util.h"
#include "../framework/util/audioloader.h"
#include "../framework/util/sfz.h"
#include <array>

namespace pt = boost::property_tree;

#define MIDI_NOTES 128
#define SAMPLER_INDENTIFIER "midicube_sample_player"

class Sampler;
struct SamplerVoice;

struct ModulateableProperty {
	double value = 0;
	unsigned int cc = 0;
	double cc_amount = 0;
	double cc_multiplier = 1;
	double velocity_amount = 0;

	inline double apply_modulation(SamplerVoice* voice, Sampler* sampler);

	void load(boost::property_tree::ptree& parent, std::string path, double def) {
		auto tree = parent.get_child_optional(path);
		if (tree && (tree.get().get_child_optional("value") || tree.get().get_child_optional("cc_amount") || tree.get().get_child_optional("velocity_amount"))) {
			load(tree.get(), def);
		}
		else {
			value = parent.get<double>(path, def);
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
	ModulateableProperty attack{0.001};
	ModulateableProperty decay{0};
	ModulateableProperty sustain{1};
	ModulateableProperty release{0.001};

	ADSREnvelopeShape attack_shape = ADSREnvelopeShape::LINEAR_ADSR;
	ADSREnvelopeShape pre_decay_shape = ADSREnvelopeShape::EXPONENTIAL_ADSR;
	ADSREnvelopeShape decay_shape = ADSREnvelopeShape::EXPONENTIAL_ADSR;
	ADSREnvelopeShape release_shape = ADSREnvelopeShape::EXPONENTIAL_ADSR;

	ModulateableProperty hold{0};
	ModulateableProperty pre_decay{0};

	ModulateableProperty attack_hold{0};
	ModulateableProperty peak_volume{1};
	ModulateableProperty decay_volume{1};
	ModulateableProperty sustain_time{0};
	ModulateableProperty release_volume{0};

	bool pedal_catch = false;

	inline ADSREnvelopeData apply(SamplerVoice* voice, Sampler* sampler) {
		ADSREnvelopeData env;
		env.attack = attack.apply_modulation(voice, sampler);
		env.decay = decay.apply_modulation(voice, sampler);
		env.sustain = sustain.apply_modulation(voice, sampler);
		env.release = release.apply_modulation(voice, sampler);

		env.attack_shape = attack_shape;
		env.pre_decay_shape = pre_decay_shape;
		env.decay_shape = decay_shape;
		env.release_shape = release_shape;

		env.hold = hold.apply_modulation(voice, sampler);
		env.pre_decay =  pre_decay.apply_modulation(voice, sampler);

		env.attack_hold = attack_hold.apply_modulation(voice, sampler);
		env.peak_volume = peak_volume.apply_modulation(voice, sampler);
		env.decay_volume = decay_volume.apply_modulation(voice, sampler);
		env.sustain_time = sustain_time.apply_modulation(voice, sampler);
		env.release_volume = release_volume.apply_modulation(voice, sampler);
		return env;
	}
};

struct SampleControl {
	unsigned int cc = 0;
	double default_value = 0;
	std::string name = "Control";
	bool save = false;
};

struct SamplePreset {
	std::string name;
	unsigned int preset_number = 0;
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
	ModulateableProperty start{0};
	unsigned int loop_start = 0;
	unsigned int loop_end = 0;
	unsigned int loop_crossfade = 0;
};

struct SampleRegion {
	unsigned int min_velocity = 0;
	unsigned int max_velocity = 127;
	unsigned int min_note = 0;
	unsigned int max_note = 127;
	std::unordered_map<unsigned int, ControlTrigger> control_triggers;
	double note = 60;
	ModulateableProperty volume{1};
	ModulateableProperty amplitude{1};
	double pitch_keytrack = 1;
	double release_decay = 1;
	unsigned int min_preset = 0;
	unsigned int max_preset = 0;

	LoopedAudioSample sample;
	LoopType loop = NO_LOOP;

	SampleFilter filter;
	SampleEnvelope env;

	TriggerType trigger = TriggerType::ATTACK_TRIGGER;

	SampleRegion() {

	};
};

struct SamplerVoice : public TriggeredNote {
	double unirand = 0;
	double birand = 0;
	bool alternate = false;
	double time = 0;
	bool hit_loop = false;
	double layer_amp = 1;
	SampleRegion* region = nullptr;
	LoopedAudioSample* sample = nullptr;
	WaveTableADSREnvelope env;
	Filter lfilter;
	Filter rfilter;
};

struct SampleRegionIndex {
	std::vector<unsigned int> controls = {};
	std::array<std::array<std::vector<SampleRegion*>, MIDI_NOTES>, MIDI_NOTES> velocities;
};

class SampleSound {
public:
	std::string default_path = "./";
	std::string name = "Sample";
	double volume = 1;
	std::vector<SampleControl> controls;
	std::vector<SamplePreset> presets = {};
	std::vector<SampleRegion*> samples = {};

	SampleSound();

	~SampleSound();

};

class SamplerProgram : public PluginProgram {
public:
	std::string sound_name;
	std::unordered_map<unsigned int, double> controls;

	virtual std::string get_plugin_name();
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

#define SAMPLER_POLYPHONY 64

class Sampler : public SoundEngine<SamplerVoice, SAMPLER_POLYPHONY> {

private:
	SampleSound* sample;
	SampleRegionIndex index;
	SampleSoundStore& store;

	void set_sample (SampleSound* sample);

	unsigned int preset_number = 0;
	size_t preset_index = 0;
	bool alternate = false;

public:
	std::array<double, MIDI_CONTROL_COUNT> cc;

	void control_change(unsigned int control, unsigned int value);

	Sampler(PluginHost& h, Plugin& plugin, SampleSoundStore& s);

	void process_note_sample(const SampleInfo& info, SamplerVoice& note, size_t note_index);

	bool note_finished(const SampleInfo& info, SamplerVoice& note, size_t note_index);

	void press_note(const SampleInfo& info, unsigned int note, double velocity);

	void release_note(const SampleInfo& info, unsigned int note, double velocity);

	std::string get_name();

	SampleSound* get_sound() {
		return sample;
	}

	ssize_t get_sound_index();

	void set_sound_index(ssize_t index);

	void save_program(PluginProgram **prog);

	void apply_program(PluginProgram *prog);

	~Sampler();

	size_t get_preset_index() const {
		return preset_index;
	}

	void set_preset_index(size_t presetIndex = 0) {
		preset_index = presetIndex;
		if (sample) {
			preset_number = sample->presets[presetIndex].preset_number;
		}
	}

	inline double get_cc_value(unsigned int cc, SamplerVoice* voice) {
		const KeyboardEnvironment& env = get_host().get_environment();

		if (cc <= 127) {
			return this->cc[cc];
		}
		else {
			switch (cc) {
			case 128:
				return (env.pitch_bend_percent * 0.5) + 0.5;
			case 129:
				return env.aftertouch;
			case 130:
				return env.aftertouch; //FIXME support poly at
			case 131:
				return voice->velocity; //Note on velocity
			case 132:
				return voice->velocity; //Note off velocity FIXME
			case 133:
				return voice->note/127.0;
			case 134:
				return get_status().pressed_notes > 0;
			case 135:
				return voice->unirand;
			case 136:
				return voice->birand;
			case 137:
				return voice->alternate;
			default:
				return 0;
			}
		}
	}
};

extern SampleSound* load_sound(std::string file, std::string folder, StreamedAudioPool& pool);

extern void save_sound(std::string file);

inline double ModulateableProperty::apply_modulation(SamplerVoice *voice,
		Sampler *sampler) {
	double val = value;
	val += voice->velocity * velocity_amount;
	double cc_val = sampler->get_cc_value(cc, voice);
	val += cc_val * cc_amount;
	if (cc_multiplier != 1) {
		val *= (1 - cc_val) + cc_val * cc_multiplier;
	}
	return val;
}

class SamplerPlugin : public Plugin {
private:
	SampleSoundStore store;

public:
	SamplerPlugin() : Plugin({
		"B3 Organ",
		SAMPLER_INDENTIFIER,
		PluginType::PLUGIN_TYPE_SOUND_ENGINE,
		0,
		2,
		true,
		false
	}) {
		store.load_sounds("./data/samples"); //FIXME dynamic path
		store.pool.start();
	}

	PluginProgram* create_program() {
		return new SamplerProgram();
	}
	PluginInstance* create(PluginHost *host) {
		return new Sampler(*host, *this, store);
	}

	~SamplerPlugin() {
		store.pool.stop();
	}

};


#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
