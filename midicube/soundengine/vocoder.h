/*
 * vocoder.h
 *
 *  Created on: 11 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_VOCODER_H_
#define MIDICUBE_SOUNDENGINE_VOCODER_H_

#include "soundengine.h"

class Vocoder : public SoundEngine {

private:
	DelayBuffer delay;
	bool delay_enabled = true;
	double delay_time = 0.5;
	unsigned int delay_repetition = 4;

public:
	Vocoder();

	void process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note);

	void process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info);

	void control_change(unsigned int control, unsigned int value);

	std::string get_name();

	~Vocoder();
};

#endif /* MIDICUBE_SOUNDENGINE_VOCODER_H_ */
