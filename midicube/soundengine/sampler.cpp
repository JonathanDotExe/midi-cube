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


double SampleSound::get_sample(unsigned int channel, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
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
	sample *= envelope.amplitude(info.time, note, env);
	return sample;
}

bool SampleSound::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	return this->envelope.is_finished(info.time, note, env);
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
	sample = load_sound("./data/samples/piano");
}

void Sampler::process_note_sample(std::array<double, OUTPUT_CHANNELS>& channels, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	for (size_t channel = 0; channel < channels.size(); ++channel) {
		channels[channel] += this->sample->get_sample(channel, info, note, env) * note.velocity;
	}
}

bool Sampler::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env) {
	return this->sample->note_finished(info, note, env);
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
		env.attack = j.at("attack").get<double>();
		env.decay = j.at("decay").get<double>();
		env.sustain = j.at("sustain").get<double>();
		env.release = j.at("release").get<double>();
		//Parse
		SampleSound* sound = new SampleSound();
		sound->set_envelope(env);
		try {
			//Load sounds
			for (auto r : j["regions"].get<std::vector<json>>()) {
				SampleRegion* region = new SampleRegion();
				try {
					region->freq = note_to_freq(r.at("note").get<int>());
					std::string file = folder + "/" + r.at("file").get<std::string>();
					if (!read_audio_file(region->sample, file)) {
						throw std::runtime_error("Couldn't load sample file " + file);
					}
				}
				catch (json::exception& e) {
					delete region;
					region = nullptr;
					throw e;
				}
				catch (std::exception& e) {
					delete region;
					region = nullptr;
					throw e;
				}
				sound->push_sample(region);
			}
		}
		catch (json::exception& e) {
			delete sound;
			sound = nullptr;
			throw e;
		}
		catch (std::exception& e) {
			delete sound;
			sound = nullptr;
			throw e;
		}
		return sound;
	}
	else {
		std::cerr << "Couldn't load sample sound" << std::endl;
	}
	return nullptr;
}

