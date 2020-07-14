/*
 * vocoder.cpp
 *
 *  Created on: 11 Jul 2020
 *      Author: jojo
 */

#include "vocoder.h"
#include <algorithm>

Vocoder::Vocoder() {
	double step = (double) (VOCODER_HIGH_BAND - VOCODER_LOW_BAND)/(carrier_filters.size());
	for (size_t i = 0; i < carrier_filters.size(); ++i) {
		carrier_filters[i].set_low_cutoff(VOCODER_LOW_BAND + step * (i + i));
		carrier_filters[i].set_high_cutoff(VOCODER_LOW_BAND + step * i);
		modulator_filters[i].set_low_cutoff(VOCODER_LOW_BAND + step * (i + i));
		modulator_filters[i].set_high_cutoff(VOCODER_LOW_BAND + step * i);
	}
}

void Vocoder::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note) {
	double sample = saw_wave(info.time - note.phase_shift, note.freq);
	sample *= 0.3 * data.vocoder_amp;
	for (size_t channel = 0; channel < channels.size(); ++channel) {
		channels[channel] += sample;
	}
}

void Vocoder::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info) {
	//Vocoder
	if (std::any_of(channels.begin(), channels.end(), [] (double s) {return s != 0;})) {
		std::array<double, OUTPUT_CHANNELS> carrier = {};
		std::array<double, OUTPUT_CHANNELS> modulator = {};
		std::array<double, OUTPUT_CHANNELS> filtered = {};
		double input = info.input_sample;
		for (size_t i = 0; i < carrier_filters.size(); ++i) {
			carrier = channels;
			for (size_t j = 0; j < modulator.size(); ++j) {
				modulator[j] = input;
			}
			carrier_filters[i].apply(carrier, info.time_step);
			modulator_filters[i].apply(modulator, info.time_step);
			for (size_t j = 0; j < filtered.size(); ++j) {
				filtered[j] = carrier[j] * modulator[j];
			}
		}
		channels = filtered;
		for (size_t i = 0; i < channels.size(); ++i) {
			channels[i] *= 0.3 * data.vocoder_amp;
		}
	}

	//Voice
	double voice = info.input_sample;
	if (data.delay)  {
		unsigned int del = data.delay_time * info.sample_rate;
		delay.add_sample(voice, del, data.delay_feedback, del, data.delay_mul);
	}
	voice += delay.process();
	voice *= data.voice_amp;
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += voice;
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

