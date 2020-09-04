/*
 * looper.cpp
 *
 *  Created on: Sep 4, 2020
 *      Author: jojo
 */

#include "looper.h"

void Looper::apply(std::array<double, OUTPUT_CHANNELS>& channels, Metronome& metronome, SampleInfo& info) {
	if (preset.on) {
		size_t index = info.sample_time % (preset.bars * info.sample_rate * 4 * 60 / metronome.get_bpm()); //Assuming its a 4/4 measure

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
