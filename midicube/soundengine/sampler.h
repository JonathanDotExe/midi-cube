/*
 * sampler.h
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_SAMPLER_H_
#define MIDICUBE_SOUNDENGINE_SAMPLER_H_

#include "soundengine.h"
#include "audiofile.h"

struct SampleRegion {
	AudioSample attack_sample;
	AudioSample sustain_sample;
	AudioSample release_sample;
	unsigned int note;
	unsigned int low_note;
	unsigned int high_note;

};

class SampleSound {
private:
	std::vector<SampleRegion*> samples;

public:

	double get_sample(SampleInfo& info, TriggeredNote& note);

	void push_sample(SampleRegion* region);

	~SampleSound();

};

class Sampler : public SoundEngine {

private:
	SampleSound* sample;

public:

	Sampler(SampleSound* sample);

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note) {

	}

	bool note_finished(SampleInfo& info, TriggeredNote& note);

	std::string get_name();

	~Sampler();

};

#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
