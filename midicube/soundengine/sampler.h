/*
 * sampler.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SAMPLER_H_
#define MIDICUBE_SOUNDENGINE_SAMPLER_H_

#include <unordered_map>
#include "soundengine.h"
#include "../audiofile.h"
#include "../envelope.h"

struct SampleRegionConfig {
	std::string filename;
	double freq;
};

struct SampleSoundConfig {
	ADSREnvelope envelope;
	std::vector<SampleRegionConfig> regions;
};


struct SampleRegion {
	AudioSample sample;
	double freq;


	SampleRegion () {
		sample.clear();
		freq = 0;
	};

};

class SampleSound {
private:
	std::vector<SampleRegion*> samples;
	ADSREnvelopeData envelope;

public:

	SampleSound();

	double get_sample(unsigned int channel, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env);

	void push_sample(SampleRegion* region);

	ADSREnvelopeData get_envelope();

	void set_envelope(ADSREnvelopeData env);

	~SampleSound();

};

class SampleSoundStore {
private:
	std::unordered_map<std::string, SampleSound*> samples;
public:

	SampleSound* get_sound(std::string name);

	void load_sounds(std::string folder);

	void load_sound(std::string folder);

	~SampleSoundStore();
};

class Sampler : public BaseSoundEngine {

private:
	SampleSound* sample;
	std::array<ADSREnvelope, SOUND_ENGINE_POLYPHONY> envs;

public:

	Sampler();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	std::string get_name();

	~Sampler();

};

extern SampleSound* load_sound(std::string folder);


#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
