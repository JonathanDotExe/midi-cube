/*
 * looper.cpp
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#include "looper.h"
#include <cstddef>
#include <algorithm>

void LooperChannel::apply(double& lout, double& rout, Metronome& metronome, SampleInfo& info) {
	//Play
	if (play) {
		if (reset) {
			lbuffer = {};
			rbuffer = {};
			reset = false;
		}

		unsigned int bpm = metronome.get_bpm();
		size_t index = info.sample_time % (preset.bars * info.sample_rate * 4 * 60 / std::max(bpm, (unsigned int) 1)); //FIXME Assuming its a 4/4 measure

		if (index < LOOPER_BUFFER_SIZE) {
			lout += lbuffer[index];
			rout += rbuffer[index];
		}
	}
}

void LooperChannel::record(double lin, double rin, Metronome& metronome, SampleInfo& info) {
	unsigned int bpm = metronome.get_bpm();
	size_t index = info.sample_time % (preset.bars * info.sample_rate * 4 * 60 / std::max(bpm, (unsigned int) 1)); //FIXME Assuming its a 4/4 measure

	if (index < LOOPER_BUFFER_SIZE) {
		//Record
		lbuffer[index] += lin;
		rbuffer[index] += rin;
	}
}

void Looper::apply(double& lsample, double& rsample, Metronome& metronome, SampleInfo& info) {
	if (active) {
		double l = lsample;
		double r = rsample;
		if (solo_channel >= 0) {
			channels[record_channel].apply(lsample, rsample, metronome, info);
		}
		else {
			for (size_t i = 0; i < LOOPER_CHANNELS; ++i) {
				channels[i].apply(lsample, rsample, metronome, info);
			}
		}
		if (record_channel >= 0) {
			channels[record_channel].record(l, r, metronome, info);
		}
	}
}

