/*
 * sampler.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SAMPLER_H_
#define MIDICUBE_SOUNDENGINE_SAMPLER_H_

#include "../../framework/core/plugins/soundengine.h"
#include "../../framework/util/audiofile.h"
#include "../../framework/dsp/envelope.h"
#include "../../framework/util/util.h"
#include "../../framework/util/audioloader.h"
#include "../../framework/util/sfz.h"
#include <array>

namespace pt = boost::property_tree;

#define MIDI_NOTES 128
#define SAMPLER_INDENTIFIER "midicube_sample_player"

class Sampler;
struct SamplerVoice;

struct SamplerCCModulation {
	unsigned int cc = 0;
	double amount = 0;
	double smooth = 0;
	bool multiply = false;
};

struct ModulateableProperty {
	double value = 0;
	double velocity_amount = 0;
	std::vector<SamplerCCModulation> cc{};

	inline double apply_modulation(SamplerVoice* voice, Sampler* sampler, std::unordered_map<SamplerCCModulation*, PortamendoBuffer> cc_portamendos);

	void load(boost::property_tree::ptree& parent, std::string path, double def) {
		auto tree = parent.get_child_optional(path);
		if (tree && (tree.get().get_child_optional("value") || tree.get().get_child_optional("cc") || tree.get().get_child_optional("velocity_amount"))) {
			load(tree.get(), def);
		}
		else {
			value = parent.get<double>(path, def);
		}
	}

	void load(boost::property_tree::ptree& tree, double def) {
		value = tree.get("value", def);
		velocity_amount = tree.get("velocity_amount", velocity_amount);
		if (tree.get_child_optional("cc")) {
			for (auto& cc : tree.get_child("cc")) {
				SamplerCCModulation mod;
				mod.cc = cc.second.get("cc", 128);
				mod.amount = cc.second.get("amount", 0.0);
				mod.multiply = cc.second.get("multiply", false);
				this->cc.push_back(mod);
			}
		}
	}

	void add_smooth_cc(std::vector<SamplerCCModulation*>* mod) {
		for (SamplerCCModulation& m : cc) {
			if (m.smooth > 0) {
				mod->push_back(&m);
			}
		}
	}

};

struct ModulatableADSREnvelopeData {
	ModulateableProperty attack{0.0};
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

	inline ADSREnvelopeData apply(SamplerVoice* voice, Sampler* sampler, std::unordered_map<SamplerCCModulation*, PortamendoBuffer> cc_portamendos) {
		ADSREnvelopeData env;
		env.attack = attack.apply_modulation(voice, sampler, cc_portamendos);
		env.decay = decay.apply_modulation(voice, sampler, cc_portamendos);
		env.sustain = sustain.apply_modulation(voice, sampler, cc_portamendos);
		env.release = release.apply_modulation(voice, sampler, cc_portamendos);

		env.attack_shape = attack_shape;
		env.pre_decay_shape = pre_decay_shape;
		env.decay_shape = decay_shape;
		env.release_shape = release_shape;

		env.hold = hold.apply_modulation(voice, sampler, cc_portamendos);
		env.pre_decay =  pre_decay.apply_modulation(voice, sampler, cc_portamendos);

		env.attack_hold = attack_hold.apply_modulation(voice, sampler, cc_portamendos);
		env.peak_volume = peak_volume.apply_modulation(voice, sampler, cc_portamendos);
		env.decay_volume = decay_volume.apply_modulation(voice, sampler, cc_portamendos);
		env.sustain_time = sustain_time.apply_modulation(voice, sampler, cc_portamendos);
		env.release_volume = release_volume.apply_modulation(voice, sampler, cc_portamendos);
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
	ModulatableADSREnvelopeData env;
	ModulateableProperty velocity_amount{1.0};
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
	ModulateableProperty pitch{0};
	ModulateableProperty volume{1};
	ModulateableProperty amplitude{1};
	ModulateableProperty pan{0.0};
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

	std::vector<SamplerCCModulation*> get_smooth_mod();
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
	std::unordered_map<SamplerCCModulation*, PortamendoBuffer> cc_portamendos;
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

	void set_sample (SampleSound* sample);

	unsigned int preset_number = 0;
	size_t preset_index = 0;
	bool alternate = false;

public:
	SampleSoundStore& store;
	std::array<double, MIDI_CONTROL_COUNT> cc;

	void control_change(unsigned int control, unsigned int value);

	Sampler(PluginHost& h, Plugin& plugin, SampleSoundStore& s);

	void process_note_sample(const SampleInfo& info, SamplerVoice& note, size_t note_index);

	void process_sample(const SampleInfo &info);

	bool note_finished(const SampleInfo& info, SamplerVoice& note, size_t note_index);

	void press_note(const SampleInfo& info, unsigned int note, unsigned int channel, double velocity);

	void release_note(const SampleInfo& info, unsigned int note, unsigned int channel, double velocity);

	std::string get_name();

	SampleSound* get_sound() {
		return sample;
	}

	ssize_t get_sound_index();

	void set_sound_index(ssize_t index);

	void save_program(PluginProgram **prog);

	void apply_program(PluginProgram *prog);

	Menu* create_menu();

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
		if (cc <= 127) {
			return this->cc[cc];
		}
		else {
			switch (cc) {
			case 128:
				return (host_environment.pitch_bend_percent * 0.5) + 0.5;
			case 129:
				return host_environment.aftertouch;
			case 130:
				return host_environment.aftertouch; //TODO support poly at
			case 131:
				return voice->velocity; //Note on velocity
			case 132:
				return voice->velocity; //Note off velocity TODO
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
		Sampler *sampler, std::unordered_map<SamplerCCModulation*, PortamendoBuffer> cc_portamendos) {
	double val = value;
	val += voice->velocity * velocity_amount;
	for (SamplerCCModulation& mod : cc) {
		double cc_val = 0;
		if (mod.smooth > 0 && mod.cc <= 127) {
			cc_val = cc_portamendos[&mod].get();
		}
		else {
			cc_val = sampler->get_cc_value(mod.cc, voice);
		}

		if (mod.multiply) {
			val *= (1 - cc_val) + cc_val * mod.amount;
		}
		else {
			val += cc_val * mod.amount;
		}
	}
	return val;
}

class SamplerPlugin : public Plugin {
private:
	SampleSoundStore store;

public:
	SamplerPlugin() : Plugin({
		"Sampler",
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
