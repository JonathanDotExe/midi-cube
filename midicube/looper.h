/*
 * looper.h
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_LOOPER_H_
#define MIDICUBE_SOUNDENGINE_LOOPER_H_

#include <array>
#include "audio.h"
#include "metronome.h"

//16 MB
#define LOOPER_BUFFER_SIZE 16777216

struct LooperPreset {
	unsigned int bars = 4;
	bool on = false;
};

class Looper {
private:
	std::array<std::array<double, OUTPUT_CHANNELS>, LOOPER_BUFFER_SIZE> buffer = {};

public:
	LooperPreset preset;
	bool play = true;
	bool record = true;

	void apply(std::array<double, OUTPUT_CHANNELS>& channels, Metronome& metronome, SampleInfo& info);

};


#endif /* MIDICUBE_SOUNDENGINE_LOOPER_H_ */
