/*
 * sampler.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "sampler.h"
#include <iostream>


//SampleSound
double SampleSound::get_sample(unsigned int channel, SampleInfo& info, TriggeredNote& note) {
	//Find region
	SampleRegion* region = nullptr;
	for (size_t i = 0; i < samples.size() && !region; ++i) {
		if (samples[i]->high_freq >= note.freq) {
			region = samples[i];
		}
	}
	if (region == nullptr) {
		region =  samples.at(samples.size() - 1);
	}
	//Play sound
	//TODO use sustain and release samples as well
	return region->attack_sample.sample(channel, (info.time - note.start_time + note.phase_shift) * note.freq/region->freq, info.sample_rate);
}

bool SampleSound::note_finished(SampleInfo& info, TriggeredNote& note) {
	//Find region
	/*SampleRegion* region = nullptr;
	for (size_t i = 0; i < samples.size() && !region; ++i) {
		if (samples[i]->high_freq >= note.freq) {
			region = samples[i];
		}
	}
	if (region == nullptr) {
		region = samples.at(samples.size() - 1);
	}
	//Calculate length
	//TODO use sustain and release samples as well
	double time = (info.time - note.start_time + note.phase_shift) * note.freq/region->freq;
	return (region->attack_sample.duration() + region->sustain_sample.duration() + region->release_sample.duration()) < time;*/
	return !note.pressed;
}

void SampleSound::push_sample(SampleRegion* region) {
	samples.push_back(region);
}

SampleSound::~SampleSound() {
	for (size_t i = 0; i < samples.size(); ++i) {
		delete samples[i];
	}
	samples.clear();
}

//Sampler
Sampler::Sampler() {
	sample = new SampleSound();
	SampleRegion* reg;
	//C1
	reg = new SampleRegion();
	read_audio_file(reg->attack_sample, "./data/samples/piano/c1.wav");
	reg->freq = note_to_freq(24);
	reg->low_freq = note_to_freq(0);
	reg->high_freq = note_to_freq(30);
	sample->push_sample(reg);
	//C2
	reg = new SampleRegion();
	read_audio_file(reg->attack_sample, "./data/samples/piano/c2.wav");
	reg->freq = note_to_freq(36);
	reg->low_freq = note_to_freq(31);
	reg->high_freq = note_to_freq(42);
	sample->push_sample(reg);
	//C3
	reg = new SampleRegion();
	read_audio_file(reg->attack_sample, "./data/samples/piano/c3.wav");
	reg->freq = note_to_freq(48);
	reg->low_freq = note_to_freq(43);
	reg->high_freq = note_to_freq(54);
	sample->push_sample(reg);
	//C4
	reg = new SampleRegion();
	read_audio_file(reg->attack_sample, "./data/samples/piano/c4.wav");
	reg->freq = note_to_freq(60);
	reg->low_freq = note_to_freq(55);
	reg->high_freq = note_to_freq(66);
	sample->push_sample(reg);
	//C5
	reg = new SampleRegion();
	read_audio_file(reg->attack_sample, "./data/samples/piano/c5.wav");
	reg->freq = note_to_freq(72);
	reg->low_freq = note_to_freq(67);
	reg->high_freq = note_to_freq(78);
	sample->push_sample(reg);
	//C6
	reg = new SampleRegion();
	read_audio_file(reg->attack_sample, "./data/samples/piano/c6.wav");
	reg->freq = note_to_freq(84);
	reg->low_freq = note_to_freq(79);
	reg->high_freq = note_to_freq(90);
	sample->push_sample(reg);
	//C7
	reg = new SampleRegion();
	read_audio_file(reg->attack_sample, "./data/samples/piano/c7.wav");
	reg->freq = note_to_freq(96);
	reg->low_freq = note_to_freq(91);
	reg->high_freq = note_to_freq(102);
	sample->push_sample(reg);
	//C8
	reg = new SampleRegion();
	read_audio_file(reg->attack_sample, "./data/samples/piano/c8.wav");
	reg->freq = note_to_freq(108);
	reg->low_freq = note_to_freq(103);
	reg->high_freq = note_to_freq(127);
	sample->push_sample(reg);
}

void Sampler::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note) {
	for (size_t channel = 0; channel < channels.size(); ++channel) {
		channels[channel] += this->sample->get_sample(channel, info, note);
	}
}

bool Sampler::note_finished(SampleInfo& info, TriggeredNote& note) {
	bool f = this->sample->note_finished(info, note);
	return f;
}

std::string Sampler::get_name() {
	return "Sampler";
}

Sampler::~Sampler() {
	delete sample;
	sample = nullptr;
}

