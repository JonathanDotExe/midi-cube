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
	ADSREnvelope envelope;

public:

	SampleSound();

	double get_sample(unsigned int channel, SampleInfo& info, TriggeredNote& note);

	bool note_finished(SampleInfo& info, TriggeredNote& note);

	void push_sample(SampleRegion* region);

	ADSREnvelope get_envelope();

	void set_envelope(ADSREnvelope env);

	~SampleSound();

};

class Sampler : public SoundEngine {

private:
	SampleSound* sample;

public:

	Sampler();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, size_t note_index);

	bool note_finished(SampleInfo& info, TriggeredNote& note);

	std::string get_name();

	~Sampler();

};

#endif /* MIDICUBE_SOUNDENGINE_SAMPLER_H_ */
