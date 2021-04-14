/*
 * looper.cpp
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#include "looper.h"
#include <cstddef>
#include <algorithm>

void LooperChannel::apply(double lin, double rin, double& lout, double& rout, Metronome& metronome, SampleInfo& info, bool record) {
	if (preset.on) {
		if (reset) {
			lbuffer = {};
			rbuffer = {};
			reset = false;
		}

		unsigned int bpm = metronome.get_bpm();
		size_t index = info.sample_time % (preset.bars * info.sample_rate * 4 * 60 / std::max(bpm, (unsigned int) 1)); //FIXME Assuming its a 4/4 measure

		if (index < LOOPER_BUFFER_SIZE) {
			//Play
			if (play) {
				lout += lbuffer[index];
				rout += rbuffer[index];
			}
			//Record
			if (record) {
				lbuffer[index] += lin;
				rbuffer[index] += rin;
			}
		}
	}
}

void Looper::apply(double& lsample, double& rsample, Metronome& metronome, SampleInfo& info) {
	if (active) {
		double l = lsample;
		double r = rsample;
		for (size_t i = 0; i < LOOPER_CHANNELS; ++i) {
			channels[i].apply(l, r, lsample, rsample, metronome, info, (ssize_t) i == record_channel);
		}
	}
}

