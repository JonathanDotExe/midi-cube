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

enum LoopType {
	NO_LOOP, ATTACK_LOOP, ALWAYS_LOOP
};

struct SampleFilter {
	FilterType filter_type = FilterType::LP_12;
	double filter_cutoff = 1;
	double filter_resonance = 0;
	double filter_kb_track = 0;
	unsigned int filter_kb_track_note = 36;
	double filter_velocity_amount = 0.0;
};

struct SampleEnvelope {
	ADSREnvelopeData env = {0, 0, 1, 0};
	double amp_velocity_amount = 0.0;
	bool sustain_entire_sample = false;
};

struct LoopedAudioSample {
	AudioSample sample;
	unsigned int loop_start = 0;
	unsigned int loop_duration = 0;
	unsigned int loop_crossfade = 0;
};

struct SampleZone {
	LoopedAudioSample sample;
	LoopedAudioSample sustain_sample;
	LoopType loop = NO_LOOP;


	double layer_velocity_amount = 0.0;
	double freq = 0;
	double max_freq = 0;
	ssize_t filter = -1;
	ssize_t env = -1;

	SampleZone () {
		sample.sample.clear();
		sustain_sample.sample.clear();
	};

};

struct SampleVelocityLayer {
	double max_velocity;
	std::vector<SampleZone*> zones = {};

	~SampleVelocityLayer() {
		for (SampleZone* zone : zones) {
			delete zone;
		}
		zones.clear();
	}
};

struct SamplerVoice : public TriggeredNote {
	double time = 0;
	bool hit_loop = false;
	double layer_amp = 1;
	SampleZone* zone = nullptr;
	LoopedAudioSample* sample = nullptr;
	SampleEnvelope* env_data = nullptr;
	SampleFilter* filter = nullptr;
	LinearADSREnvelope env;
	Filter lfilter;
	Filter rfilter;
};

class SampleSound {
public:
	std::string name = "Sample";
	double volume = 1;
	std::vector<SampleEnvelope> envelopes = {};
	std::vector<SampleFilter> filters = {};
	std::vector<SampleVelocityLayer*> samples = {};

	SampleSound();

	void get_sample(double freq, double velocity, SamplerVoice& voic, bool sustain);

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

public:

	Sampler();

	void process_note_sample(double& lsample, double& rsample, SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index);

	bool note_finished(SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index);

	void press_note(SampleInfo& info, unsigned int note, double velocity);

	void release_note(SampleInfo& info, unsigned int note);

	std::string get_name();

	ssize_t get_sound_index();

	void set_sound_index(ssize_t index);

	void save_program(EngineProgram **prog);

	void apply_program(EngineProgram *prog);

	~Sampler();

};

extern SampleSound* load_sound(std::string folder);

extern void save_sound(std::string file);


#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
