/*
 * vocoder.cpp
 *
 *  Created on: 11 Jul 2020
 *      Author: jojo
 */

#include "vocoder.h"

Vocoder::Vocoder() {
	// TODO Auto-generated constructor stub

}

void Vocoder::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note) {

}

void Vocoder::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	double sample = info.input_sample;
	if (delay_enabled)  {
		unsigned int del = delay_time * info.sample_rate;
		delay.add_sample(sample, del, delay_repetition, del, 0.5);
	}
	sample += delay.process();
	sample *= 0.3;
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += sample;
	}
}

void Vocoder::control_change(unsigned int control, unsigned int value) {

}

std::string Vocoder::get_name() {
	return "Vocoder";
}

Vocoder::~Vocoder() {

}

