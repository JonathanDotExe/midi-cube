/*
 * vocoder.cpp
 *
 *  Created on: 11 Jul 2020
 *      Author: jojo
 */

#include "vocoder.h"
#include <algorithm>

VocoderData::VocoderData() {
	double step = (double) (VOCODER_HIGH_BAND - VOCODER_LOW_BAND)/(carrier_filters.size());
	for (size_t i = 0; i < carrier_filters.size(); ++i) {
		carrier_filters[i].set_low_cutoff(VOCODER_LOW_BAND + step * (i + i));
		carrier_filters[i].set_high_cutoff(VOCODER_LOW_BAND + step * i);
		modulator_filters[i].set_low_cutoff(VOCODER_LOW_BAND + step * (i + i));
		modulator_filters[i].set_high_cutoff(VOCODER_LOW_BAND + step * i);
	}
}


Vocoder::Vocoder() {

}

void Vocoder::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, SoundEngineData& d, size_t note_index) {
	VocoderData& data = dynamic_cast<VocoderData&>(d);
	double sample = saw_wave(info.time - note.phase_shift, note.freq);
	sample *= data.preset.vocoder_amp;
	for (size_t channel = 0; channel < channels.size(); ++channel) {
		channels[channel] += sample;
	}
}

void Vocoder::process_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status, SoundEngineData& d) {
	VocoderData& data = dynamic_cast<VocoderData&>(d);
	double sample = 0;
	//Vocoder
	if (std::any_of(channels.begin(), channels.end(), [] (double s) {return s != 0;})) {
		double carrier = channels.at(0);
		double modulator = 0;
		sample += carrier * data.preset.carrier_amp;
		double filtered = 0;
		double input = info.input_sample;
		std::cout << info.input_sample << std::endl;
		for (size_t i = 0; i < data.carrier_filters.size(); ++i) {
			carrier = channels.at(0);
			modulator = input;
			carrier = data.carrier_filters[i].apply(carrier, info.time_step);
			modulator = data.modulator_filters[i].apply(modulator, info.time_step);
			filtered += carrier * modulator;
		}
		sample += filtered * data.preset.vocoder_amp;
	}

	//Voice
	double voice = info.input_sample * data.preset.voice_amp;
	sample += voice;
	if (data.preset.delay)  {
		unsigned int del = data.preset.delay_time * info.sample_rate;
		data.delay.add_sample(sample, del, data.preset.delay_feedback, del, data.preset.delay_mul);
	}
	sample += data.delay.process();

	channels = {};
	for (size_t i = 0; i < channels.size(); ++i) {
		channels[i] += sample;
	}
}

void Vocoder::control_change(unsigned int control, unsigned int value, SoundEngineData& d) {
	VocoderData& data = dynamic_cast<VocoderData&>(d);
	if (control == data.preset.delay_control) {
		data.preset.delay = value > 0;
	}
	if (control == data.preset.delay_time_control) {
		data.preset.delay_time = value/127.0 * 10.0;
	}
	if (control == data.preset.delay_feedback_control) {
		data.preset.delay_feedback = value/4;
	}
	if (control == data.preset.delay_mul_control) {
		data.preset.delay_mul = value/127.0;
	}
	if (control == data.preset.vocoder_amp_control) {
		data.preset.vocoder_amp = value/127.0;
	}
	if (control == data.preset.voice_amp_control) {
		data.preset.voice_amp = value/127.0;
	}
	if (control == data.preset.carrier_amp_control) {
		data.preset.carrier_amp = value/127.0;
	}
}

std::string Vocoder::get_name() {
	return "Vocoder";
}

Vocoder::~Vocoder() {

}

