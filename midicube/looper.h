/*
 * looper.h
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_SOUNDENGINE_LOOPER_H_
#define MIDICUBE_SOUNDENGINE_LOOPER_H_

#include <array>
#include <atomic>
#include "audio.h"
#include "metronome.h"

//16 MB
#define LOOPER_BUFFER_SIZE 1048576

struct LooperPreset {
	std::atomic<unsigned int> bars{4};
	std::atomic<bool> on{false};
};

class Looper {
private:
	std::array<double, LOOPER_BUFFER_SIZE> lbuffer = {};
	std::array<double, LOOPER_BUFFER_SIZE> rbuffer = {};


public:
	LooperPreset preset;
	std::atomic<bool> play{true};
	std::atomic<bool> record{true};
	std::atomic<bool> reset{false};	//true will invoke a reset next sample

	void apply(double& lsample, double& rsample, Metronome& metronome, SampleInfo& info);
};


#endif /* MIDICUBE_SOUNDENGINE_LOOPER_H_ */
