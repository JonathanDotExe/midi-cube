/*
 * sampler.cpp
 *
 *  Created on: 5 Jul 2020
 *      Author: jojo
 */

#include "sampler.h"
#include <iostream>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;


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
		region1 =  samples[samples.size() - 1];
	}
	if (!region2) {
		region2 =  samples[samples.size() - 1];
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
	return sample;
}

void SampleSound::push_sample(SampleRegion* region) {
	samples.push_back(region);
}

ADSREnvelopeData SampleSound::get_envelope() {
	return envelope;
}

void SampleSound::set_envelope(ADSREnvelopeData env) {
	this->envelope = env;
}

SampleSound::~SampleSound() {
	for (size_t i = 0; i < samples.size(); ++i) {
		delete samples[i];
	}
	samples.clear();
}

//SampleSoundStore
SampleSound* SampleSoundStore::get_sound(std::string name) {
	return samples[name];
}

void SampleSoundStore::load_sounds(std::string folder) {
	//TODO
}

void SampleSoundStore::load_sound(std::string folder) {
	//TODO
}

SampleSoundStore::~SampleSoundStore() {
	for (std::pair<std::string, SampleSound*> s : samples) {
		delete s.second;
	}
}

//Sampler
Sampler::Sampler() {
	sample = load_sound("./data/samples/piano");
}

void Sampler::process_note_sample(double& lsample, double& rsample, SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	ADSREnvelopeData e = this->sample->get_envelope();
	ADSREnvelope& en = envs[note_index];
	if (en.is_finished()) {
		en.reset();
	}

	double vol = en.amplitude(e, info.time_step, note.pressed, env.sustain);
	lsample += this->sample->get_sample(0, info, note, env) * note.velocity * vol;
	rsample += this->sample->get_sample(1, info, note, env) * note.velocity * vol;
}

bool Sampler::note_finished(SampleInfo& info, TriggeredNote& note, KeyboardEnvironment& env, size_t note_index) {
	return !note.pressed && envs[note_index].is_finished();
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
	pt::ptree tree;
	SampleSound* sound = nullptr;
	try {
		pt::read_json(folder + "/sound.json", tree);

		//Envelope
		ADSREnvelopeData env;
		env.attack = tree.get<double>("attack", 0.0005);
		env.decay = tree.get<double>("decay", 0.0);
		env.sustain = tree.get<double>("sustain", 1);
		env.release = tree.get<double>("release", 0.003);
		//Parse
		sound = new SampleSound();
		sound->set_envelope(env);
		try {
			//Load sounds
			for (auto r : tree.get_child("regions")) {
				SampleRegion* region = new SampleRegion();
				try {
					region->freq = note_to_freq(r.second.get<int>("note", 0));
					std::string file = folder + "/" + r.second.get<std::string>("file");
					if (!read_audio_file(region->sample, file)) {
						throw std::runtime_error("Couldn't load sample file " + file);
					}
				}
				catch (std::exception& e) {
					delete region;
					region = nullptr;
					throw e;
				}
				sound->push_sample(region);
			}
		}
		catch (std::exception& e) {
			delete sound;
			sound = nullptr;
			throw e;
		}
	}
	catch (pt::json_parser_error& e) {
		std::cerr << "Couldn't load sound.json" << std::endl;
	}
	return sound;
}

template<>
std::string get_engine_name<Sampler>() {
	return "Sampler";
}

void __fix_link_sampler_name__ () {
	get_engine_name<Sampler>();
}

