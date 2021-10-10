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
#include "framework/core/metronome.h"

//16 MB
#define LOOPER_CHANNELS 8
#define LOOPER_BUFFER_SIZE 1048576

struct LooperPreset {
	unsigned int bars{4};
};

class LooperChannel {
private:
	std::array<double, LOOPER_BUFFER_SIZE> lbuffer = {};
	std::array<double, LOOPER_BUFFER_SIZE> rbuffer = {};


public:
	LooperPreset preset;
	bool play{true};
	bool reset{false};	//true will invoke a reset next sample

	void apply(double& lout, double& rout, Metronome& metronome, SampleInfo& info);

	void record(double lin, double rin, Metronome& metronome, SampleInfo& info);
};

class Looper {
public:
	std::array<LooperChannel, LOOPER_CHANNELS> channels;
	bool active = false;
	ssize_t record_channel = -1;
	ssize_t solo_channel = -1;

	void apply(double& lsample, double& rsample, Metronome& metronome, const SampleInfo& info);

};

#endif /* MIDICUBE_SOUNDENGINE_LOOPER_H_ */
