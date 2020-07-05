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

struct SampleRegion {
	AudioSample attack_sample;
	AudioSample sustain_sample;
	AudioSample release_sample;
	double freq;
	double low_freq;
	double high_freq;

	SampleRegion () {
		attack_sample.clear();
		sustain_sample.clear();
		release_sample.clear();
		freq = 0;
		low_freq = 0;
		high_freq = 0;
	};

};

class SampleSound {
private:
	std::vector<SampleRegion*> samples;

public:

	double get_sample(unsigned int channel, SampleInfo& info, TriggeredNote& note);

	bool note_finished(SampleInfo& info, TriggeredNote& note);

	void push_sample(SampleRegion* region);

	~SampleSound();

};

class Sampler : public SoundEngine {

private:
	SampleSound* sample;

public:

	Sampler();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note);

	bool note_finished(SampleInfo& info, TriggeredNote& note);

	std::string get_name();

	~Sampler();

};

#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
