/*
 * looper.cpp
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#include "looper.h"
#include <algorithm>

void Looper::apply(std::array<double, OUTPUT_CHANNELS>& channels, Metronome& metronome, SampleInfo& info) {
	if (preset.on) {
		if (reset) {
			buffer = {};
			reset = false;
		}

		unsigned int bpm = metronome.get_bpm();
		size_t index = info.sample_time % (preset.bars * info.sample_rate * 4 * 60 / std::max(bpm, (unsigned int) 1)); //Assuming its a 4/4 measure

		if (index < buffer.size()) {
			std::array<double, OUTPUT_CHANNELS> ch = channels;
			//Play
			if (play) {
				for (size_t i = 0; i < channels.size(); ++i) {
					channels[i] += buffer[index].at(i);
				}
			}
			//Record
			if (record) {
				for (size_t i = 0; i < channels.size(); ++i) {
					buffer[index].at(i) += ch[i];
				}
			}
		}
	}
}

