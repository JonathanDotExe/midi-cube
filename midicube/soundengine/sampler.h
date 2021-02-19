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

struct SampleZone {
	AudioSample sample;
	double freq = 0;
	double max_freq = 0;
	ADSREnvelopeData env = {0, 0, 1, 0};

	SampleZone () {
		sample.clear();
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

class SampleSound {
public:
	std::string name = "Sample";
	std::vector<SampleVelocityLayer*> samples = {};

	SampleSound();

	SampleZone* get_sample(double freq, double velocity);

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

#define SAMPLER_POLYPHONY 64

struct SamplerVoice : public TriggeredNote {
	SampleZone* zone = nullptr;
	ADSREnvelope env;
};

class Sampler : public BaseSoundEngine<SamplerVoice, SAMPLER_POLYPHONY> {

private:
	SampleSound* sample;

public:

	Sampler();

	void process_note_sample(double& lsample, double& rsample, SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index);

	bool note_finished(SampleInfo& info, SamplerVoice& note, KeyboardEnvironment& env, size_t note_index);

	std::string get_name();

	~Sampler();

};

extern SampleSound* load_sound(std::string folder);


#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
