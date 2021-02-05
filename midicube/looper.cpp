/*
 * looper.cpp
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#include "looper.h"
#include <algorithm>

void Looper::apply(double& lsample, double& rsample, Metronome& metronome, SampleInfo& info) {
	if (preset.on) {
		if (reset) {
			lbuffer = {};
			rbuffer = {};
			reset = false;
		}

		unsigned int bpm = metronome.get_bpm();
		size_t index = info.sample_time % (preset.bars * info.sample_rate * 4 * 60 / std::max(bpm, (unsigned int) 1)); //Assuming its a 4/4 measure

		if (index < LOOPER_BUFFER_SIZE) {
			double l = lsample;
			double r = rsample;
			//Play
			if (play) {
				lsample += lbuffer[index];
				rsample += rbuffer[index];
			}
			//Record
			if (record) {
				lbuffer[index] += l;
				rbuffer[index] += r;
			}
		}
	}
}

