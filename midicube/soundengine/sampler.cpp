/*
 * sampler.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "sampler.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


//SampleSound
SampleSound::SampleSound() {
	envelope = {0, 0, 1, 0};
}


double SampleSound::get_sample(unsigned int channel, SampleInfo& info, TriggeredNote& note) {
	//Find regions
	SampleRegion* region1 = nullptr;
	SampleRegion* region2 = nullptr;
	for (size_t i = 1; i < samples.size() && !region1 && !region2; ++i) {
		if (samples[i]->freq >= note.freq) {
			region1 = samples[i - 1];
			region2 = samples[i];
		}
	}
	if (!region1) {
		region1 =  samples.at(samples.size() - 1);
	}
	if (!region2) {
		region2 =  samples.at(samples.size() - 1);
	}
	double prog = 0;
	if (region2->freq != region1->freq) {
		prog = (note.freq - region1->freq)/(region2->freq - region1->freq);
	}
	else {
		prog = 0;
	}
	//Play sound
	//TODO use sustain and release samples as well
	double time = (info.time - note.start_time + note.phase_shift);
	double sample = 0;
	if (prog != 1) {
		sample = region2->sample.isample(channel, time * note.freq/region2->freq, info.sample_rate);
	}
	sample = region1->sample.isample(channel, time * note.freq/region1->freq, info.sample_rate) * (1 -prog) +
			region2->sample.isample(channel, time * note.freq/region2->freq, info.sample_rate) * (prog);
	sample *= envelope.amplitude(info.time, note);
	return sample;
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
	return !note.pressed && note.release_time + envelope.release < info.time;
}

void SampleSound::push_sample(SampleRegion* region) {
	samples.push_back(region);
}

ADSREnvelope SampleSound::get_envelope() {
	return envelope;
}

void SampleSound::set_envelope(ADSREnvelope env) {
	this->envelope = env;
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
	sample->set_envelope({0.005, 0, 1, 0.2});
	SampleRegion* reg;
	//C1
	reg = new SampleRegion();
	read_audio_file(reg->sample, "./data/samples/piano/c1.wav");
	reg->freq = note_to_freq(24);
	sample->push_sample(reg);
	//C2
	reg = new SampleRegion();
	read_audio_file(reg->sample, "./data/samples/piano/c2.wav");
	reg->freq = note_to_freq(36);
	sample->push_sample(reg);
	//C3
	reg = new SampleRegion();
	read_audio_file(reg->sample, "./data/samples/piano/c3.wav");
	reg->freq = note_to_freq(48);
	sample->push_sample(reg);
	//C4
	reg = new SampleRegion();
	read_audio_file(reg->sample, "./data/samples/piano/c4.wav");
	reg->freq = note_to_freq(60);
	sample->push_sample(reg);
	//C5
	reg = new SampleRegion();
	read_audio_file(reg->sample, "./data/samples/piano/c5.wav");
	reg->freq = note_to_freq(72);
	sample->push_sample(reg);
	//C6
	reg = new SampleRegion();
	read_audio_file(reg->sample, "./data/samples/piano/c6.wav");
	reg->freq = note_to_freq(84);
	sample->push_sample(reg);
	//C7
	reg = new SampleRegion();
	read_audio_file(reg->sample, "./data/samples/piano/c7.wav");
	reg->freq = note_to_freq(96);
	sample->push_sample(reg);
	//C8
	reg = new SampleRegion();
	read_audio_file(reg->sample, "./data/samples/piano/c8.wav");
	reg->freq = note_to_freq(108);
	sample->push_sample(reg);
}

void Sampler::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, size_t note_index) {
	for (size_t channel = 0; channel < channels.size(); ++channel) {
		channels[channel] += this->sample->get_sample(channel, info, note) * note.velocity * 0.3;
	}
}

bool Sampler::note_finished(SampleInfo& info, TriggeredNote& note) {
	return this->sample->note_finished(info, note);
}

std::string Sampler::get_name() {
	return "Sampler";
}

Sampler::~Sampler() {
	delete sample;
	sample = nullptr;
}


extern SampleSound* load_sound(std::string folder) {
	//Load file
	std::ifstream def_file;
	def_file.open(folder + "/sound.json");
	if (def_file.is_open()) {
		//Parse json
		std::string json_text ((std::istreambuf_iterator<char>(def_file)), std::istreambuf_iterator<char>());
		auto j = json::parse(json_text);
		//Envelope
		ADSREnvelope env;
		env.attack = j["attack"].get<double>();
		env.decay = j["decay"].get<double>();
		env.sustain = j["sustain"].get<double>();
		env.release = j["release"].get<double>();
		//Load sounds
		for (auto s : j["regions"].get<std::vector<json>>()) {
			//TODO load
		}
		//Parse
		SampleSound* sound = new SampleSound();
		sound->set_envelope(env);
		return sound;
	}
	else {
		std::cerr << "Couldn't load sample sound" << std::endl;
	}
	return nullptr;
}

