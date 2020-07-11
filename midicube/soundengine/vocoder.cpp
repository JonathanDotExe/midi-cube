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
	if (data.delay)  {
		unsigned int del = data.delay_time * info.sample_rate;
		delay.add_sample(sample, del, data.delay_feedback, del, data.delay_mul);
	}
	sample += delay.process();
	sample *= 0.3;
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += sample;
	}
}

void Vocoder::control_change(unsigned int control, unsigned int value) {
	if (control == data.delay_control) {
		data.delay = value > 0;
	}
	if (control == data.delay_time_control) {
		data.delay_time = (double) value/127.0 * 10.0;
	}
	if (control == data.delay_feedback_control) {
		data.delay_feedback = value/4;
	}
	if (control == data.delay_mul_control) {
		data.delay_mul = (double) value/127;
	}
}

std::string Vocoder::get_name() {
	return "Vocoder";
}

Vocoder::~Vocoder() {

}

