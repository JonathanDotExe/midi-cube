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
	ADSREnvelopeData envelope = {0, 0, 1, 0};

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
	std::vector<SampleVelocityLayer*> samples = {};


	SampleSound();

	double get_sample(unsigned int channel, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env);

	void push_sample(SampleZone* region);

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

#define SAMPLER_POLYPHONY 64

class Sampler : public BaseSoundEngine<TriggeredNote, SAMPLER_POLYPHONY> {

private:
	SampleSound* sample;
	std::array<ADSREnvelope, SAMPLER_POLYPHONY> envs;

public:

	Sampler();

	void process_note_sample(double& lsample, double& rsample, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	bool note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index);

	std::string get_name();

	~Sampler();

};

extern SampleSound* load_sound(std::string folder);


#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
